#include "LoginManager.h"
#include <iostream>

LoginManager::LoginManager(IDatabase* database) : m_database(database)
{
}

LoginManager::~LoginManager()
{
}

/*
* if failed return 0
* if succussed return 201
*/
int LoginManager::signup(
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
    if (m_database->doesUserExist(username)) {
        std::cerr << "Signup failed: Username '" << username << "' already exists" << std::endl;
        throw std::exception("Username already exists");
    }

    // Add the new user
    bool result = m_database->addNewUser(username, password, email, street, aptNumber, city, phone, birthDate);

    if (!result) {
        std::cerr << "Signup failed: Database error adding user '" << username << "'" << std::endl;
        throw std::exception("Database error creating new user");
    }

    std::cout << "User created successfully: " << username << std::endl;
    return 201; // Success code
}

/*
* if failed return 0
* if succussed return 200
*/
int LoginManager::login(const std::string& username, const std::string& password)
{
    // user can't be connected if he not exist
    if (!m_database->doesUserExist(username)) return 0;

    // Check password
    if (!this->m_database->doesPasswordMatch(username, password)) return 0;

    // Adding user
    this->m_loggedUsers.push_back(LoggedUser(username));

    return 200;
}

void LoginManager::logout(std::string username)
{
    for (auto it = m_loggedUsers.begin(); it != m_loggedUsers.end(); ++it)
    {
        if (it->getUsername() == username)
        {
            m_loggedUsers.erase(it);
            return;
        }
    }
}

