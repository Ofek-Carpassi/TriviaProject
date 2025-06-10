#pragma once
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include "GameManager.h"
#include "RoomManager.h"

class HeadToHeadManager
{
public:
    HeadToHeadManager(GameManager* gameManager, RoomManager* roomManager);
    ~HeadToHeadManager();

    // Add player to queue and get matching status
    bool joinQueue(const std::string& username, std::string& opponent, unsigned int& roomId);

    // Check if player has a match
    bool checkMatch(const std::string& username, std::string& opponent, unsigned int& roomId);

    // Remove player from queue
    void leaveQueue(const std::string& username);

    // Get number of players in queue
    unsigned int getQueueSize();

private:
    std::vector<std::string> m_waitingPlayers;
    std::map<std::string, std::pair<std::string, unsigned int>> m_matchedPlayers;
    std::mutex m_mutex;
    GameManager* m_gameManager;
    RoomManager* m_roomManager;
};