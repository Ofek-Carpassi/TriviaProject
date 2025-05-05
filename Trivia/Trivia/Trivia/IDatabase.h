#pragma once
#include <string>
#include <vector>
#include "Question.h"

class IDatabase
{
public:
    virtual ~IDatabase() = default;

    // Connection methods
    virtual bool open() = 0;
    virtual void close() = 0;

    // User management methods
    virtual bool doesUserExist(const std::string& username) = 0;
    virtual bool doesPasswordMatch(const std::string& username, const std::string& password) = 0;
	virtual bool addNewUser(const std::string& username, const std::string& password, const std::string& email, const std::string& street, const std::string& aptNumber, const std::string& city, const std::string& phone, const std::string& birthDate) = 0;
    virtual std::vector<Question> getQuestions(int questionsAmount) const = 0;
    virtual int getNumOfCorrectAnswers(string username) const = 0;
    virtual int getNumOfTotalAnswers(string username) const = 0;
    virtual int getNumOfPlayerGames(string username) const = 0;
    virtual int getPlayerScore(string username) const = 0;
    virtual wordList getHighScores() const = 0;
	virtual double getAverageAnswerTime(const std::string& username) const = 0;

	// Encryption methods
    virtual bool saveUserCryptoKey(const std::string& userId, const std::string& key, size_t position) = 0;
    virtual bool getUserCryptoKey(const std::string& userId, std::string& keyOut, size_t& positionOut) = 0;
};