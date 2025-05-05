#pragma once
#include <vector>
#include <map>
#include <string>
#include "Room.hpp"

class RoomManager
{
public:
    RoomManager();
    ~RoomManager();

    // management
    unsigned int createRoom(const std::string& creator, const std::string& roomName, unsigned int maxPlayers, unsigned int timePerQuestion);
    bool deleteRoom(unsigned int roomId);

    // getters
    std::vector<RoomData> getRooms() const;
    Room* getRoom(unsigned int roomId);
    RoomState getRoomState(unsigned int roomId) const;

private:
    std::map<unsigned int, Room*> m_rooms;
    unsigned int m_nextRoomId;
};

