#include "GameManager.h"
#include <iostream>
#include <algorithm>

GameManager::GameManager(IDatabase* database) : m_database(database)
{
}

GameManager::~GameManager()
{
}

GetQuestionResponse GameManager::getQuestion(const std::string& username)
{
    GetQuestionResponse response;

    // Check if player is in a game
    if (m_playerGameData.find(username) == m_playerGameData.end())
    {
        response.status = 0; // No game found for player
        return response;
    }

    PlayerGameData& playerData = m_playerGameData[username];

    // Check if player has finished all questions
    if (playerData.currentQuestionIndex >= playerData.totalQuestions || playerData.hasFinished)
    {
        // Mark player as finished
        playerData.hasFinished = true;

        // Check if all players in the room are done
        updateGameFinishedStatus(playerData.roomId);

        response.status = 0; // No more questions
        return response;
    }

    // Get the current question
    response.status = 1; // Success, has question
    response.question = playerData.currentQuestion.getQuestion();

    // Add all answers to the response
    const std::vector<std::string>& answers = playerData.currentQuestion.getPossibleAnswers();
    for (unsigned int i = 0; i < static_cast<unsigned int>(answers.size()); i++)
    {
        response.answers[i] = answers[i];
    }

    playerData.currentQuestionIndex++; // Move to next question for next time

    // If this was the last question, get a new one from the database for next time
    if (playerData.currentQuestionIndex < playerData.totalQuestions)
    {
        // Get next question from database (for next time)
        std::vector<Question> nextQuestions = m_database->getQuestions(1);
        if (!nextQuestions.empty())
        {
            playerData.currentQuestion = nextQuestions[0];
        }
    }

    return response;
}

bool GameManager::submitAnswer(const std::string& username, unsigned int answerId, double answerTime)
{
    // Check if player is in a game
    if (m_playerGameData.find(username) == m_playerGameData.end())
    {
        throw std::exception("Player is not in a game");
    }

    PlayerGameData& playerData = m_playerGameData[username];

    // Check if the answer is correct
    bool isCorrect = (answerId == playerData.currentQuestion.getCorrectAnswerId());

    // Update player statistics
    if (isCorrect)
    {
        playerData.correctAnswerCount++;
    }
    else
    {
        playerData.wrongAnswerCount++;
    }

    playerData.totalAnswerTime += static_cast<float>(answerTime);
    playerData.answeredQuestions++;

    // Check if player has finished all questions
    if (playerData.currentQuestionIndex >= playerData.totalQuestions)
    {
        playerData.hasFinished = true;
        updateGameFinishedStatus(playerData.roomId);
    }

    return isCorrect;
}

GetGameResultsResponse GameManager::getGameResults(unsigned int roomId)
{
    GetGameResultsResponse response;

    // Check if the game is finished
    if (!m_gameFinished[roomId])
    {
        response.status = 0; // Game not finished yet
        return response;
    }

    response.status = 1; // Game finished, return results

    // Get all players for this room
    auto it = m_roomPlayers.find(roomId);
    if (it == m_roomPlayers.end())
    {
        return response;
    }

    // Get results for each player
    for (const std::string& username : it->second)
    {
        if (m_playerGameData.find(username) != m_playerGameData.end())
        {
            const PlayerGameData& playerData = m_playerGameData[username];

            PlayerResultsResponse result;
            result.username = username;
            result.correctAnswerCount = playerData.correctAnswerCount;
            result.wrongAnswerCount = playerData.wrongAnswerCount;
            result.averageAnswerTime = getAverageAnswerTime(username);

            response.results.push_back(result);
        }
    }

    return response;
}

void GameManager::playerLeftGame(const std::string& username)
{
    // Find the player's game
    auto it = m_playerGameData.find(username);
    if (it != m_playerGameData.end())
    {
        // Even if a player leaves, we keep their data for results
        // but mark them as finished
        it->second.hasFinished = true;

        // Update game status for the room
        updateGameFinishedStatus(it->second.roomId);
    }
}

void GameManager::createGame(unsigned int roomId, const std::vector<std::string>& players, unsigned int questionCount)
{
    // Get questions from the database
    std::vector<Question> questions = m_database->getQuestions(questionCount);

    if (questions.empty())
    {
        throw std::exception("Failed to get questions from database");
    }

    // Store players in the room
    m_roomPlayers[roomId] = players;
    m_gameFinished[roomId] = false;

    // Initialize game data for each player
    for (const std::string& username : players)
    {
        PlayerGameData playerData;
        playerData.roomId = roomId;
        playerData.currentQuestionIndex = 0;
        playerData.totalQuestions = questionCount;
        playerData.currentQuestion = questions[0]; // Start with first question
        playerData.correctAnswerCount = 0;
        playerData.wrongAnswerCount = 0;
        playerData.totalAnswerTime = 0;
        playerData.answeredQuestions = 0;
        playerData.hasFinished = false;

        m_playerGameData[username] = playerData;
    }
}

float GameManager::getAverageAnswerTime(const std::string& username) const
{
    auto it = m_playerGameData.find(username);
    if (it != m_playerGameData.end() && it->second.answeredQuestions > 0)
    {
        return it->second.totalAnswerTime / it->second.answeredQuestions;
    }
    return 0.0f;
}

bool GameManager::isGameFinished(unsigned int roomId) const
{
    // Get all players for this room
    auto roomPlayersIt = m_roomPlayers.find(roomId);
    if (roomPlayersIt == m_roomPlayers.end())
    {
        return true; // No players, so game is finished
    }

    // Check if all players have finished their questions
    for (const std::string& username : roomPlayersIt->second)
    {
        auto playerIt = m_playerGameData.find(username);
        if (playerIt != m_playerGameData.end() && !playerIt->second.hasFinished)
        {
            return false; // At least one player hasn't finished
        }
    }

    return true; // All players have finished
}

void GameManager::updateGameFinishedStatus(unsigned int roomId)
{
    bool finished = isGameFinished(roomId);
    if (finished && !m_gameFinished[roomId])
    {
        m_gameFinished[roomId] = true;
        std::cout << "Game in room " << roomId << " has finished!" << std::endl;
    }
}