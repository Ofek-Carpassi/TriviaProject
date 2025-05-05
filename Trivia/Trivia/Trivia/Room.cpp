#include "Room.h"
#include <algorithm>

Room::Room(unsigned int id, const std::string& roomName, unsigned int maxPlayers, unsigned int timePerQuestion, const std::string& creator)
{
    m_metadata.id = id;
    m_metadata.name = roomName;
    m_metadata.maxPlayers = maxPlayers;
    m_metadata.timePerQuestion = timePerQuestion;
    m_metadata.isActive = false;
    m_metadata.state = RoomState::WAITING;

    // add the creator
    m_users.push_back(creator);
}

Room::~Room()
{
}

RoomData Room::getRoomData() const
{
    return m_metadata;
}

std::vector<std::string> Room::getAllUsers() const
{
    return m_users;
}

bool Room::isActive() const
{
    return m_metadata.isActive;
}

bool Room::addUser(const std::string& username)
{
    // if room is full
    if (m_users.size() >= m_metadata.maxPlayers)
        return false;

    // if user is already in the room
    auto it = std::find(m_users.begin(), m_users.end(), username);
    if (it != m_users.end())
        return false;

    // add user to the room
    m_users.push_back(username);
    return true;
}

bool Room::removeUser(const std::string& username)
{
    auto it = std::find(m_users.begin(), m_users.end(), username);
    if (it != m_users.end())
    {
        m_users.erase(it);
        return true;
    }
    return false;
}

void Room::setActive(bool isActive)
{
    m_metadata.isActive = isActive;
}

void Room::setState(RoomState state)
{
    m_metadata.state = state;
}
