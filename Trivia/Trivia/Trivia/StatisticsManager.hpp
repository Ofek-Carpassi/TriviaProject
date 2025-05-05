#pragma once
#include "IDatabase.h"
#include "ResponseStructs.h"
#include <string>
#include <vector>
#include <algorithm>

class StatisticsManager
{
public:
    StatisticsManager(IDatabase* database);
    ~StatisticsManager();

    // Get statistics for a specific player
    PlayerStatisticsResponse getPlayerStatistics(const std::string& username);

    // Get high scores (top 5 players)
    std::vector<PlayerStatisticsResponse> getHighScores();

private:
    IDatabase* m_database;
};
