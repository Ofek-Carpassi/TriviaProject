#include "HeadToHeadManager.h"

HeadToHeadManager::HeadToHeadManager(GameManager* gameManager, RoomManager* roomManager)
    : m_gameManager(gameManager), m_roomManager(roomManager)
{
}

HeadToHeadManager::~HeadToHeadManager()
{
}

bool HeadToHeadManager::joinQueue(const std::string& username, std::string& opponent, unsigned int& roomId)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Check if player is already in queue
    auto it = std::find(m_waitingPlayers.begin(), m_waitingPlayers.end(), username);
    if (it != m_waitingPlayers.end())
    {
        return false;
    }

    // If queue is empty, add player to queue
    if (m_waitingPlayers.empty())
    {
        m_waitingPlayers.push_back(username);
        return false;
    }

    // If queue has players, match with the first one
    std::string otherPlayer = m_waitingPlayers[0];
    m_waitingPlayers.erase(m_waitingPlayers.begin());

    // Create a room for the match
    roomId = m_roomManager->createRoom(username, "Head-to-Head Match", 2, 20);
    Room* room = m_roomManager->getRoom(roomId);
    if (room)
    {
        room->addUser(otherPlayer);
        room->setActive(true);
        room->setState(RoomState::GAME_IN_PROGRESS);

        // Initialize the game
        std::vector<std::string> players = { username, otherPlayer };
        m_gameManager->createGame(roomId, players, 5); // 5 questions for head-to-head matches

        // Store the match information for both players
        m_matchedPlayers[username] = std::make_pair(otherPlayer, roomId);
        m_matchedPlayers[otherPlayer] = std::make_pair(username, roomId);

        opponent = otherPlayer;
        return true;
    }

    return false;
}

bool HeadToHeadManager::checkMatch(const std::string& username, std::string& opponent, unsigned int& roomId)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_matchedPlayers.find(username);
    if (it != m_matchedPlayers.end())
    {
        opponent = it->second.first;
        roomId = it->second.second;
        return true;
    }

    return false;
}

void HeadToHeadManager::leaveQueue(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Remove from waiting list
    auto it = std::find(m_waitingPlayers.begin(), m_waitingPlayers.end(), username);
    if (it != m_waitingPlayers.end())
    {
        m_waitingPlayers.erase(it);
    }

    // Remove from matched players
    m_matchedPlayers.erase(username);
}

unsigned int HeadToHeadManager::getQueueSize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_waitingPlayers.size();
}