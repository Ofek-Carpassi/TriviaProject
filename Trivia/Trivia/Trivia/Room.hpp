#pragma once
#include <string>
#include <vector>
#include "RoomData.hpp"

class Room
{
public:
    Room(unsigned int id, const std::string& roomName, unsigned int maxPlayers, unsigned int timePerQuestion, const std::string& creator);
    ~Room();

    // getters
    RoomData getRoomData() const;
    std::vector<std::string> getAllUsers() const;
    bool isActive() const;

    // managment
    bool addUser(const std::string& username);
    bool removeUser(const std::string& username);
    void setActive(bool isActive);
    void setState(RoomState state);

private:
    RoomData m_metadata;
    std::vector<std::string> m_users;
};

