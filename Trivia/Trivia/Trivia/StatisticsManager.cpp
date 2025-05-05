#include "StatisticsManager.hpp"

StatisticsManager::StatisticsManager(IDatabase* database) : m_database(database)
{
}

StatisticsManager::~StatisticsManager()
{
}

PlayerStatisticsResponse StatisticsManager::getPlayerStatistics(const std::string& username)
{
    PlayerStatisticsResponse stats;
    stats.username = username;

    // Get player statistics from database
    stats.gamesPlayed = m_database->getNumOfPlayerGames(username);
    stats.correctAnswers = m_database->getNumOfCorrectAnswers(username);
    stats.wrongAnswers = m_database->getNumOfTotalAnswers(username) - stats.correctAnswers;
    stats.averageAnswerTime = static_cast<unsigned int>(m_database->getAverageAnswerTime(username));

    return stats;
}

std::vector<PlayerStatisticsResponse> StatisticsManager::getHighScores()
{
    std::vector<PlayerStatisticsResponse> highScores;

    // Get words from database high scores query
    wordList scoreList = m_database->getHighScores();

    // Parse the score list into PlayerStatisticsResponse objects
    for (const auto& scoreEntry : scoreList)
    {
        // Example format: "username: 100"
        size_t colonPos = scoreEntry.find(": ");
        if (colonPos != std::string::npos)
        {
            std::string username = scoreEntry.substr(0, colonPos);
            int score = std::stoi(scoreEntry.substr(colonPos + 2));

            PlayerStatisticsResponse stats;
            stats.username = username;
            stats.correctAnswers = score;

            // Get additional stats for this player
            stats.gamesPlayed = m_database->getNumOfPlayerGames(username);
            stats.wrongAnswers = m_database->getNumOfTotalAnswers(username) - stats.correctAnswers;
            stats.averageAnswerTime = static_cast<unsigned int>(m_database->getAverageAnswerTime(username));

            highScores.push_back(stats);
        }
    }

    // Sort the high scores by correct answers (highest first)
    std::sort(highScores.begin(), highScores.end(),
        [](const PlayerStatisticsResponse& a, const PlayerStatisticsResponse& b) {
            return a.correctAnswers > b.correctAnswers;
        });

    return highScores;
}

