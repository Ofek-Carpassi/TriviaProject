#pragma once
#include <string>
#include <vector>

enum class RoomState
{
    WAITING,
    GAME_IN_PROGRESS
};

struct RoomData
{
    unsigned int id;
    std::string name;
    unsigned int maxPlayers;
    unsigned int questionCount;
    unsigned int timePerQuestion; // in seconds
    bool isActive;
    RoomState state;
};