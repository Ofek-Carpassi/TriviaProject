#include "RoomManager.hpp"

RoomManager::RoomManager() : m_nextRoomId(1)
{
}

RoomManager::~RoomManager()
{
    for (auto& room : m_rooms) room.second;
    m_rooms.clear();
}

unsigned int RoomManager::createRoom(
    const std::string& creator,
    const std::string& roomName,
    unsigned int maxPlayers,
    unsigned int timePerQuestion)
{
    // create a new room with the next available ID
    unsigned int roomId = m_nextRoomId++;
    Room* newRoom = new Room(roomId, roomName, maxPlayers, timePerQuestion, creator);

    // store the room
    m_rooms[roomId] = newRoom;

    return roomId;
}

bool RoomManager::deleteRoom(unsigned int roomId)
{
    auto it = m_rooms.find(roomId);
    if (it != m_rooms.end())
    {
        delete it->second;
        m_rooms.erase(it);
        return true;
    }
    return false;
}

std::vector<RoomData> RoomManager::getRooms() const
{
    std::vector<RoomData> rooms;

    for (const auto& room : m_rooms) rooms.push_back(room.second->getRoomData());

    return rooms;
}

Room* RoomManager::getRoom(unsigned int roomId)
{
    auto it = m_rooms.find(roomId);
    if (it != m_rooms.end()) return it->second;
    return nullptr;
}

RoomState RoomManager::getRoomState(unsigned int roomId) const
{
    auto it = m_rooms.find(roomId);
    if (it != m_rooms.end()) return it->second->getRoomData().state;

    // return WAITING as a default if room not found
    return RoomState::WAITING;
}

