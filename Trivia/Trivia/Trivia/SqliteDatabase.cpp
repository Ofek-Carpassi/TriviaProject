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

    initializeDefaultQuestions();

    return true;
}

void SqliteDatabase::initializeDefaultQuestions() const
{
    // Check if we already have questions
    const char* countSql = "SELECT COUNT(*) FROM QUESTIONS;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, countSql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            if (count > 0) {
                sqlite3_finalize(stmt);
                std::cout << "Questions already exist in database (" << count << " questions).\n";
                return;
            }
        }
        sqlite3_finalize(stmt);
    }

    std::cout << "Initializing database with default questions...\n";

    // Array of default questions to populate the database
    struct DefaultQuestion {
        const char* question;
        const char* correct;
        const char* wrong1;
        const char* wrong2;
        const char* wrong3;
    };

    DefaultQuestion questions[] = {
        {
            "What is the capital of France?",
            "Paris",
            "London",
            "Berlin",
            "Madrid"
        },
        {
            "Which planet is known as the Red Planet?",
            "Mars",
            "Venus",
            "Jupiter",
            "Saturn"
        },
        {
            "What is the largest mammal in the world?",
            "Blue Whale",
            "Elephant",
            "Giraffe",
            "Hippopotamus"
        },
        {
            "In which year did World War II end?",
            "1945",
            "1944",
            "1946",
            "1943"
        },
        {
            "What is the chemical symbol for gold?",
            "Au",
            "Ag",
            "Fe",
            "Cu"
        },
        {
            "Which programming language is this project's server written in?",
            "C++",
            "Java",
            "Python",
            "C#"
        },
        {
            "What is the smallest country in the world?",
            "Vatican City",
            "Monaco",
            "San Marino",
            "Liechtenstein"
        },
        {
            "Which ocean is the largest?",
            "Pacific Ocean",
            "Atlantic Ocean",
            "Indian Ocean",
            "Arctic Ocean"
        },
        {
            "What is the hardest natural substance on Earth?",
            "Diamond",
            "Quartz",
            "Steel",
            "Granite"
        },
        {
            "Which gas makes up about 78% of Earth's atmosphere?",
            "Nitrogen",
            "Oxygen",
            "Carbon Dioxide",
            "Hydrogen"
        },
        {
            "In which continent is the Sahara Desert located?",
            "Africa",
            "Asia",
            "Australia",
            "South America"
        },
        {
            "What is the currency of Japan?",
            "Yen",
            "Won",
            "Yuan",
            "Rupee"
        },
        {
            "Which instrument measures earthquakes?",
            "Seismograph",
            "Barometer",
            "Thermometer",
            "Anemometer"
        },
        {
            "What is the largest organ in the human body?",
            "Skin",
            "Liver",
            "Brain",
            "Heart"
        },
        {
            "Which vitamin is produced when skin is exposed to sunlight?",
            "Vitamin D",
            "Vitamin C",
            "Vitamin A",
            "Vitamin B12"
        },
        {
            "What is the speed of light in a vacuum?",
            "299,792,458 m/s",
            "300,000,000 m/s",
            "299,000,000 m/s",
            "301,000,000 m/s"
        },
        {
            "Which element has the atomic number 1?",
            "Hydrogen",
            "Helium",
            "Lithium",
            "Carbon"
        },
        {
            "In which year was the first iPhone released?",
            "2007",
            "2006",
            "2008",
            "2005"
        },
        {
            "What is the longest river in the world?",
            "Nile River",
            "Amazon River",
            "Yangtze River",
            "Mississippi River"
        },
        {
            "Which sport is played at Wimbledon?",
            "Tennis",
            "Cricket",
            "Football",
            "Rugby"
        }
    };

    // Insert all questions using transactions for better performance
    const char* beginTransaction = "BEGIN TRANSACTION;";
    const char* commitTransaction = "COMMIT;";
    
    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, beginTransaction, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to begin transaction: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return;
    }

    const char* insertSql = "INSERT INTO QUESTIONS (QUESTION, CORRECT_ANSWER, ANSWER2, ANSWER3, ANSWER4) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* insertStmt;
    
    if (sqlite3_prepare_v2(m_db, insertSql, -1, &insertStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    int successCount = 0;
    int totalQuestions = sizeof(questions) / sizeof(questions[0]);

    for (int i = 0; i < totalQuestions; i++) {
        // Bind parameters
        sqlite3_bind_text(insertStmt, 1, questions[i].question, -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 2, questions[i].correct, -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 3, questions[i].wrong1, -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 4, questions[i].wrong2, -1, SQLITE_STATIC);
        sqlite3_bind_text(insertStmt, 5, questions[i].wrong3, -1, SQLITE_STATIC);

        if (sqlite3_step(insertStmt) == SQLITE_DONE) {
            successCount++;
        } else {
            std::cerr << "Failed to insert question: " << questions[i].question << std::endl;
        }

        // Reset for next iteration
        sqlite3_reset(insertStmt);
    }

    sqlite3_finalize(insertStmt);

    if (sqlite3_exec(m_db, commitTransaction, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to commit transaction: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    std::cout << "Successfully added " << successCount << " out of " << totalQuestions << " questions to database.\n";
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

    // First check if we have any questions at all
    const char* countSql = "SELECT COUNT(*) FROM QUESTIONS;";
    if (sqlite3_prepare_v2(m_db, countSql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            std::cout << "Found " << count << " question(s) in the database.\n";
            if (count == 0) {
                sqlite3_finalize(stmt);
                // Add a default question if none exist
                addDefaultQuestion();

                // Check again after adding default question
                if (sqlite3_prepare_v2(m_db, countSql, -1, &stmt, nullptr) == SQLITE_OK) {
                    if (sqlite3_step(stmt) == SQLITE_ROW) {
                        count = sqlite3_column_int(stmt, 0);
                        std::cout << "After adding default: Found " << count << " question(s) in the database.\n";
                    }
                    sqlite3_finalize(stmt);
                }
            }
            else {
                sqlite3_finalize(stmt);
            }
        }
        else {
            sqlite3_finalize(stmt);
        }
    }

    // For debugging, print the table schema
    sqlite3_stmt* schemaStmt;
    const char* schemaSql = "PRAGMA table_info(QUESTIONS);";
    if (sqlite3_prepare_v2(m_db, schemaSql, -1, &schemaStmt, nullptr) == SQLITE_OK) {
        std::cout << "QUESTIONS table columns:\n";
        while (sqlite3_step(schemaStmt) == SQLITE_ROW) {
            std::cout << " - " << reinterpret_cast<const char*>(sqlite3_column_text(schemaStmt, 1)) << "\n";
        }
        sqlite3_finalize(schemaStmt);
    }

    // For debugging, print first few rows
    std::cout << "First few questions in the database:\n";
    const char* debugSql = "SELECT QUESTION_ID, QUESTION FROM QUESTIONS LIMIT 3;";
    if (sqlite3_prepare_v2(m_db, debugSql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* question = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::cout << "ID: " << id << ", Question: " << (question ? question : "NULL") << "\n";
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Debug query failed: " << sqlite3_errmsg(m_db) << "\n";
    }

    // SQL query to fetch random questions - make sure column names match exactly
    const char* sql = "SELECT QUESTION, CORRECT_ANSWER, ANSWER2, ANSWER3, ANSWER4 FROM QUESTIONS ORDER BY RANDOM() LIMIT ?;";

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << "\n";

        // Also try with quoted identifiers in case they're needed
        sql = "SELECT \"QUESTION\", \"CORRECT_ANSWER\", \"ANSWER2\", \"ANSWER3\", \"ANSWER4\" FROM QUESTIONS ORDER BY RANDOM() LIMIT ?;";
        rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Alternative query also failed: " << sqlite3_errmsg(m_db) << "\n";
            goto add_default_question;
        }
    }

    std::cout << "Statement prepared successfully\n";

    // Bind the question amount to the query
    rc = sqlite3_bind_int(stmt, 1, questionsAmount);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to bind parameter: " << sqlite3_errmsg(m_db) << "\n";
        sqlite3_finalize(stmt);
        goto add_default_question;
    }

    std::cout << "Parameter bound successfully\n";

    // Loop through the results
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Check if we have valid data
        if (sqlite3_column_text(stmt, 0) == nullptr) {
            std::cerr << "NULL question text found, skipping\n";
            continue;
        }

        // Extract data
        std::string questionText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        wordList answers;

        // First answer is the correct one
        if (sqlite3_column_text(stmt, 1) != nullptr) {
            std::string correctAnswer = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            answers.push_back(correctAnswer);
        }
        else {
            answers.push_back("Default correct answer");
        }

        // Add wrong answers
        for (int i = 2; i <= 4; i++) {
            if (sqlite3_column_text(stmt, i) != nullptr) {
                answers.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            }
            else {
                answers.push_back("No answer"); // Default if NULL
            }
        }

        // The correct answer is always at index 0 in our answers list
        const int correctIndex = 0;

        // Construct a Question object and add it to the result vector
        Question q(questionText, answers, correctIndex);
        ret_val.push_back(q);

        std::cout << "Added question: " << questionText << "\n";
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Error during step: " << sqlite3_errmsg(m_db) << "\n";
    }

    // Clean up
    sqlite3_finalize(stmt);

add_default_question:
    // If we still don't have questions, create a default one
    if (ret_val.empty()) {
        std::cout << "No question returned, adding default question.\n";
        wordList defaultAnswers = { "C++", "Java", "Python", "Ruby" };
        Question defaultQ("What programming language is this project's server written in?", defaultAnswers, 0);
        ret_val.push_back(defaultQ);
    }
    else {
        std::cout << "Retrieved " << ret_val.size() << " questions from database\n";
    }

    return ret_val;
}

void SqliteDatabase::addDefaultQuestion() const
{
    std::vector<std::string> wrongAnswers = {"Java", "Python", "Ruby"};
    
    // Use the existing addQuestion method for consistency
    const_cast<SqliteDatabase*>(this)->addQuestion(
        "What programming language is this project's server written in?",
        "C++",
        wrongAnswers
    );
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

bool SqliteDatabase::addQuestion(const std::string& question, const std::string& correct_answer, const std::vector<std::string>& wrong_answers)
{
    try
    {
        // Start a transaction
        sqlite3_exec(m_db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

        // Make sure we have exactly 3 wrong answers
        if (wrong_answers.size() < 3) {
            throw std::exception("Need at least 3 wrong answers");
        }

        // Insert the question with all answers in one statement
        std::string sql = "INSERT INTO QUESTIONS (QUESTION, CORRECT_ANSWER, ANSWER2, ANSWER3, ANSWER4) VALUES (?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
            throw std::exception("Failed to prepare question insert");

        sqlite3_bind_text(stmt, 1, question.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, correct_answer.c_str(), -1, SQLITE_STATIC);

        // Bind the wrong answers
        for (int i = 0; i < 3 && i < wrong_answers.size(); i++) {
            sqlite3_bind_text(stmt, 3 + i, wrong_answers[i].c_str(), -1, SQLITE_STATIC);
        }

        if (sqlite3_step(stmt) != SQLITE_DONE)
            throw std::exception("Failed to insert question");

        // Get the last inserted question ID
        int questionId = sqlite3_last_insert_rowid(m_db);
        sqlite3_finalize(stmt);

        // Commit the transaction
        sqlite3_exec(m_db, "COMMIT", nullptr, nullptr, nullptr);

        std::cout << "Question added successfully with ID: " << questionId << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "Failed to add question: " << e.what() << std::endl;
        sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }
}