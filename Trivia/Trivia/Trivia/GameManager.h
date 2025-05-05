#pragma once
#include "IDatabase.h"
#include "Room.h"
#include "ResponseStructs.h"
#include <string>
#include <map>
#include <vector>
#include <unordered_map>

// Structure to hold player's game data
struct PlayerGameData
{
    unsigned int roomId{ 0 };
    unsigned int currentQuestionIndex{ 0 };
    unsigned int totalQuestions{ 0 };
    Question currentQuestion;
    unsigned int correctAnswerCount{ 0 };
    unsigned int wrongAnswerCount{ 0 };
    float totalAnswerTime{ 0.0f };
    unsigned int answeredQuestions{ 0 };
    bool hasFinished{ false };

    // Add a default constructor
    PlayerGameData() : currentQuestion("", {}, 0) {}
};

class GameManager
{
public:
    GameManager(IDatabase* database);
    ~GameManager();

    // Get next question for the player
    GetQuestionResponse getQuestion(const std::string& username);

    // Submit an answer and return if it was correct
    bool submitAnswer(const std::string& username, unsigned int answerId, double answerTime);

    // Get game results for a room
    GetGameResultsResponse getGameResults(unsigned int roomId);

    // Handle when a player leaves the game
    void playerLeftGame(const std::string& username);

    // Create a new game for a room
    void createGame(unsigned int roomId, const std::vector<std::string>& players, unsigned int questionCount);

private:
    IDatabase* m_database;
    std::unordered_map<std::string, PlayerGameData> m_playerGameData;
    std::map<unsigned int, std::vector<std::string>> m_roomPlayers;
    std::map<unsigned int, bool> m_gameFinished;

    // Get average answer time for a player
    float getAverageAnswerTime(const std::string& username) const;

    // Check if all players in a room have finished the game
    bool isGameFinished(unsigned int roomId) const;

    // Update game finished status for a room
    void updateGameFinishedStatus(unsigned int roomId);
};