#include "SqliteDatabase.h"
#include <iostream>
#include <io.h>
#include <string>

// Callback function to check if a record exists
static int doesExistCallback(void* data, int argc, char** argv, char** azColName) {
    bool* exists = static_cast<bool*>(data);
    *exists = (argc > 0);
    return 0;
}

SqliteDatabase::SqliteDatabase() : m_db(nullptr), m_dbFileName("triviaDB.sqlite") {}

SqliteDatabase::~SqliteDatabase() {
    close();
}

bool SqliteDatabase::open()
{
    int res = sqlite3_open("triviaDB.sqlite", &m_db);
    if (res != SQLITE_OK)
    {
        m_db = nullptr;
        std::cerr << "Failed to open database" << std::endl;
        return false;
    }

    // Enable foreign keys support
    res = sqlite3_exec(m_db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK)
    {
        std::cerr << "Warning: Failed to enable foreign keys" << std::endl;
    }

    std::cout << "Ensuring database tables exist..." << std::endl;

    // Create version table first to track schema versions
    const char* createVersionTableSql =
        "CREATE TABLE IF NOT EXISTS DB_VERSION ("
        "id INTEGER PRIMARY KEY CHECK (id = 1), "
        "version INTEGER NOT NULL);";

    char* errMsg = nullptr;
    res = sqlite3_exec(m_db, createVersionTableSql, nullptr, nullptr, &errMsg);
    if (res != SQLITE_OK)
    {
        std::cerr << "Error creating version table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        close();
        return false;
    }

    // Check database version
    int currentVersion = 0;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, "SELECT version FROM DB_VERSION WHERE id = 1;", -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            currentVersion = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    // If no version record exists, insert version 0
    if (currentVersion == 0)
    {
        res = sqlite3_exec(m_db, "INSERT OR IGNORE INTO DB_VERSION (id, version) VALUES (1, 0);", nullptr, nullptr, &errMsg);
        if (res != SQLITE_OK)
        {
            std::cerr << "Error initializing version record: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }

    // Create base tables
    const char* createTablesSql[] = {
        // Users table with all required fields
        "CREATE TABLE IF NOT EXISTS USERS ("
        "USERNAME TEXT PRIMARY KEY NOT NULL, "
        "PASSWORD TEXT NOT NULL, "
        "EMAIL TEXT NOT NULL);",

        // Questions table
        "CREATE TABLE IF NOT EXISTS QUESTIONS ("
        "QUESTION_ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
        "QUESTION TEXT NOT NULL, "
        "CORRECT_ANSWER TEXT NOT NULL, "
        "ANSWER2 TEXT NOT NULL, "
        "ANSWER3 TEXT NOT NULL, "
        "ANSWER4 TEXT NOT NULL);",

        // Statistics table
        "CREATE TABLE IF NOT EXISTS STATISTICS ("
        "USERNAME TEXT NOT NULL, "
        "GAMES_PLAYED INTEGER NOT NULL, "
        "CORRECT_ANSWERS INTEGER NOT NULL, "
        "WRONG_ANSWERS INTEGER NOT NULL, "
        "AVERAGE_ANSWER_TIME REAL NOT NULL, "
        "PRIMARY KEY (USERNAME), "
        "FOREIGN KEY (USERNAME) REFERENCES USERS (USERNAME) "
        "ON DELETE CASCADE ON UPDATE CASCADE);",

        // Crypto keys table for OTP encryption
        "CREATE TABLE IF NOT EXISTS CRYPTO_KEYS ("
        "USERNAME TEXT NOT NULL, "
        "KEY_VALUE TEXT NOT NULL, "
        "KEY_POSITION INTEGER NOT NULL, "
        "PRIMARY KEY (USERNAME), "
        "FOREIGN KEY (USERNAME) REFERENCES USERS (USERNAME) "
        "ON DELETE CASCADE ON UPDATE CASCADE);"
    };

    // Create all base tables
    for (const char* sql : createTablesSql)
    {
        res = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
        if (res != SQLITE_OK)
        {
            std::cerr << "Error creating table: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            close();
            return false;
        }
    }

    // Apply migrations based on version
    if (currentVersion < 1)
    {
        std::cout << "Upgrading database to version 1..." << std::endl;

        // Migration for version 1: Add additional user info columns
        const char* migrationV1Sql[] = {
            "ALTER TABLE USERS ADD COLUMN STREET TEXT DEFAULT '' NOT NULL;",
            "ALTER TABLE USERS ADD COLUMN APT_NUMBER TEXT DEFAULT '' NOT NULL;",
            "ALTER TABLE USERS ADD COLUMN CITY TEXT DEFAULT '' NOT NULL;",
            "ALTER TABLE USERS ADD COLUMN PHONE TEXT DEFAULT '' NOT NULL;",
            "ALTER TABLE USERS ADD COLUMN BIRTH_DATE TEXT DEFAULT '' NOT NULL;"
        };

        // Execute each migration statement
        for (const char* sql : migrationV1Sql)
        {
            res = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
            if (res != SQLITE_OK)
            {
                // Some SQLite versions don't support ALTER TABLE ADD COLUMN IF NOT EXISTS
                // so we'll log the error but continue (columns might already exist)
                std::cerr << "Migration note: " << errMsg << std::endl;
                sqlite3_free(errMsg);
                errMsg = nullptr;
                // Don't return false here, try to continue with other migrations
            }
        }

        // Update version to 1
        res = sqlite3_exec(m_db, "UPDATE DB_VERSION SET version = 1 WHERE id = 1;", nullptr, nullptr, &errMsg);
        if (res != SQLITE_OK)
        {
            std::cerr << "Error updating database version: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            close();
            return false;
        }

        currentVersion = 1;
    }

    // For future migrations, continue the pattern:
    // if (currentVersion < 2) { ... apply version 2 migrations ... }
    // if (currentVersion < 3) { ... apply version 3 migrations ... }

    std::cout << "All necessary tables are ready." << std::endl;
    return true;
}

void SqliteDatabase::close() {
    if (m_db != nullptr) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool SqliteDatabase::doesUserExist(const std::string& username) {
    std::string query = "SELECT * FROM USERS WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;
    bool exists = false;

    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = true;
        }
    }
    sqlite3_finalize(stmt);
    return exists;
}

bool SqliteDatabase::doesPasswordMatch(const std::string& username, const std::string& password) {
    bool matches = false;

    std::string query = "SELECT * FROM USERS WHERE USERNAME='" + username + "' AND PASSWORD='" + password + "';";

    char* errMsg = nullptr;
    int result = sqlite3_exec(m_db, query.c_str(), doesExistCallback, &matches, &errMsg);

    if (result != SQLITE_OK) {
        std::cerr << "Error checking password match: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return matches;
}

bool SqliteDatabase::addNewUser(
    const std::string& username, 
    const std::string& password, 
    const std::string& email,
    const std::string& street,
    const std::string& aptNumber,
    const std::string& city,
    const std::string& phone,
    const std::string& birthDate)
{
    // Check if user already exists
    if (doesUserExist(username)) {
        std::cerr << "User already exists" << std::endl;
        return false;
    }

    // Validate inputs
    if (!UserValidator::isValidPassword(password)) {
        std::cerr << "Invalid password format. Password must be at least 8 characters and contain uppercase, lowercase, digit, and special character." << std::endl;
        return false;
    }

    if (!UserValidator::isValidEmail(email)) {
        std::cerr << "Invalid email format. Email must have format <name>@<domain>." << std::endl;
        return false;
    }

    if (!UserValidator::isValidStreet(street)) {
        std::cerr << "Invalid street format. Street must contain only uppercase and lowercase letters." << std::endl;
        return false;
    }

    if (!UserValidator::isValidApartment(aptNumber)) {
        std::cerr << "Invalid apartment number. Apartment must be a number." << std::endl;
        return false;
    }

    if (!UserValidator::isValidCity(city)) {
        std::cerr << "Invalid city format. City must contain only uppercase and lowercase letters." << std::endl;
        return false;
    }

    if (!UserValidator::isValidPhone(phone)) {
        std::cerr << "Invalid phone format. Phone number must start with 0 followed by 2 or 3 digit prefix and 7 digits." << std::endl;
        return false;
    }

    if (!UserValidator::isValidBirthDate(birthDate)) {
        std::cerr << "Invalid birth date format. Date must be in dd/mm/yyyy format." << std::endl;
        return false;
    }

    // Add the new user
    std::string query =
        "INSERT INTO USERS (USERNAME, PASSWORD, EMAIL, STREET, APT_NUMBER, CITY, PHONE, BIRTH_DATE) VALUES ('" +
        username + "', '" + password + "', '" + email + "', '" + street + "', '" + 
        aptNumber + "', '" + city + "', '" + phone + "', '" + birthDate + "');";

    char* errMsg = nullptr;
    int result = sqlite3_exec(m_db, query.c_str(), nullptr, nullptr, &errMsg);

    if (result != SQLITE_OK) {
        std::cerr << "Error adding new user: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

std::vector<Question> SqliteDatabase::getQuestions(int questionsAmount) const 
{
    std::vector<Question> ret_val;
    sqlite3_stmt* stmt;

    // Open the SQLite database
    if (m_db == nullptr) {
        std::cerr << "Database not open.\n";
        return ret_val;
    }

    // SQL query to fetch random questions
    const char* sql = "SELECT question, answer1, answer2, answer3, answer4, correct_index FROM questions ORDER BY RANDOM() LIMIT ?;";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        // Bind the question amount to the query
        sqlite3_bind_int(stmt, 1, questionsAmount);

        // Loop through the results
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string questionText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            wordList answers;
            answers.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            answers.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            answers.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            answers.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            int correctIndex = sqlite3_column_int(stmt, 5);

            // Construct a Question object and add it to the result vector
            Question q(questionText, answers, correctIndex);
            ret_val.push_back(q);
        }
    }
    else {
        std::cerr << "Failed to prepare statement.\n";
    }

    // Clean up
    sqlite3_finalize(stmt);

    return ret_val;
}

int SqliteDatabase::getNumOfCorrectAnswers(std::string username) const
{
    std::string query = "SELECT COUNT(*) FROM user_statistics WHERE user_id = ? AND is_correct = 1;";
    sqlite3_stmt* stmt;
    int count = 0;

    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return count;
}

int SqliteDatabase::getNumOfTotalAnswers(std::string username) const
{
    std::string query = "SELECT COUNT(*) FROM user_statistics WHERE user_id = ?;";
    sqlite3_stmt* stmt;
    int count = 0;

    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return count;
}

int SqliteDatabase::getNumOfPlayerGames(std::string username) const
{
    std::string query = "SELECT COUNT(*) FROM games WHERE user_id = ? AND is_completed = 1;";
    sqlite3_stmt* stmt;
    int games = 0;

    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            games = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return games;
}

int SqliteDatabase::getPlayerScore(std::string username) const
{
    // Simple scoring: Each correct answer is worth 10 points
    std::string query = "SELECT COUNT(*) * 10 FROM user_statistics WHERE user_id = ? AND is_correct = 1;";
    sqlite3_stmt* stmt;
    int score = 0;

    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            score = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return score;
}

double SqliteDatabase::getAverageAnswerTime(const std::string& username) const
{
    std::string query = "SELECT AVG(answer_time) FROM user_statistics WHERE user_id = ?;";
    sqlite3_stmt* stmt;
    double avgTime = 0.0;

    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            avgTime = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return avgTime;
}

wordList SqliteDatabase::getHighScores() const
{
    wordList highScores;
    std::string query = "SELECT user_id, COUNT(*) as correct_count "
                        "FROM user_statistics "
                        "WHERE is_correct = 1 "
                        "GROUP BY user_id "
                        "ORDER BY correct_count DESC "
                        "LIMIT 5;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            int score = sqlite3_column_int(stmt, 1);
            highScores.push_back(username + ": " + std::to_string(score));
        }
    }
    sqlite3_finalize(stmt);
    return highScores;
}

bool SqliteDatabase::saveUserCryptoKey(const std::string& userId, const std::string& key, size_t position)
{
    // First, check if a table for crypto keys exists
    const char* createCryptoTable =
        "CREATE TABLE IF NOT EXISTS CRYPTO_KEYS ("
        "USER_ID TEXT PRIMARY KEY NOT NULL, "
        "KEY_DATA BLOB NOT NULL, "
        "POSITION INTEGER NOT NULL);";

    char* errMsg = nullptr;
    int result = sqlite3_exec(m_db, createCryptoTable, nullptr, nullptr, &errMsg);

    if (result != SQLITE_OK) {
        std::cerr << "Error creating crypto keys table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    // Now save or update the key
    std::string query = "INSERT OR REPLACE INTO CRYPTO_KEYS (USER_ID, KEY_DATA, POSITION) VALUES (?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    result = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, key.data(), static_cast<int>(key.size()), SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(position));

    // Execute
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        std::cerr << "Error saving crypto key: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    return true;
}

bool SqliteDatabase::getUserCryptoKey(const std::string& userId, std::string& keyOut, size_t& positionOut)
{
    std::string query = "SELECT KEY_DATA, POSITION FROM CRYPTO_KEYS WHERE USER_ID = ?;";

    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    // Bind parameter
    sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);

    // Execute
    result = sqlite3_step(stmt);

    if (result == SQLITE_ROW) {
        // Get the key data
        const void* keyData = sqlite3_column_blob(stmt, 0);
        int keySize = sqlite3_column_bytes(stmt, 0);
        keyOut.assign(static_cast<const char*>(keyData), keySize);

        // Get the position
        positionOut = static_cast<size_t>(sqlite3_column_int64(stmt, 1));

        sqlite3_finalize(stmt);
        return true;
    }
    else {
        sqlite3_finalize(stmt);
        return false;
    }
}