#pragma once
#include "SqliteDatabase.h"
#include "LoggedUser.h"
#include "IDatabase.h"
#include <vector>

class LoginManager
{
public:
	LoginManager(IDatabase* database);
	~LoginManager();

	int signup(const std::string& username, const std::string& password, const std::string& email, const std::string& street, const std::string& aptNumber, const std::string& city, const std::string& phone, const std::string& birthDate);
	int login(const std::string& username, const std::string& password);
	void logout(std::string username);

private:
	IDatabase* m_database;
	std::vector<LoggedUser> m_loggedUsers;
};
