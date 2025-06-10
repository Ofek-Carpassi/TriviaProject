#pragma once
#include "IDatabase.h"
#include "sqlite3.h"
#include <string>
#include "UserValidator.h"

class SqliteDatabase : public IDatabase
{
public:
    SqliteDatabase();
    virtual ~SqliteDatabase();

    // IDatabase interface implementation
    virtual bool open() override;
    virtual void close() override;
    virtual bool doesUserExist(const std::string& username) override;
    virtual bool doesPasswordMatch(const std::string& username, const std::string& password) override;
    virtual bool addNewUser(const std::string& username, const std::string& password, const std::string& email, const std::string& street, const std::string& aptNumber, const std::string& city, const std::string& phone, const std::string& birthDate) override;
    virtual std::vector<Question> getQuestions(int questionsAmount) const override;
    virtual int getNumOfCorrectAnswers(string username) const override;
    virtual int getNumOfTotalAnswers(string username) const override;
    virtual int getNumOfPlayerGames(string username) const override;
    virtual int getPlayerScore(string username) const override;
    virtual wordList getHighScores() const override;
    virtual double getAverageAnswerTime(const std::string& username) const override;
    virtual bool addQuestion(const std::string& question, const std::string& correct_answer, const std::vector<std::string>& wrong_answers) override;

private:
    sqlite3* m_db;
    std::string m_dbFileName;
    void addDefaultQuestion() const;
};