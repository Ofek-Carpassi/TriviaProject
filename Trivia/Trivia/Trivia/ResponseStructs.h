#pragma once
#include <string>
#include <vector>
#include "RoomData.h"
#include <map>

// base response structure
struct Response
{
    unsigned int status = 0;
    std::string message;
};

// room-related responses
typedef struct GetRoomsResponse
{
    unsigned int status;
    std::vector<RoomData> rooms;
} GetRoomsResponse;

typedef struct GetPlayersInRoomResponse
{
    unsigned int status;
    std::vector<std::string> players;
} GetPlayersInRoomResponse; 

// statistics-related responses
typedef struct PlayerStatisticsResponse
{
    std::string username;
    unsigned int gamesPlayed;
    unsigned int correctAnswers;
    unsigned int wrongAnswers;
    unsigned int averageAnswerTime; // in seconds
} PlayerStatisticsResponse;

typedef struct HighScoreResponse
{
    unsigned int status;
    std::vector<PlayerStatisticsResponse> statistics;
} HighScoreResponse;

typedef struct GetRoomStateResponse
{
	unsigned int status = 0;
	bool hasGameBegun = false;
    std::vector<std::string> players;
    unsigned int answerCount = 0;
    unsigned int answerTimeout = 0;
} GetRoomStateResponse;

// Game-related responses
typedef struct PlayerResultsResponse
{
    std::string username;
    unsigned int correctAnswerCount;
    unsigned int wrongAnswerCount;
    float averageAnswerTime;
} PlayerResultsResponse;

typedef struct GetGameResultsResponse
{
    unsigned int status = 0;
    std::vector<PlayerResultsResponse> results;
} GetGameResultsResponse;

typedef struct GetQuestionResponse
{
    unsigned int status = 0;
    std::string question;
    std::map<unsigned int, std::string> answers;
} GetQuestionResponse;

typedef struct SubmitAnswerResponse
{
    unsigned int status = 0;
} SubmitAnswerResponse;

typedef struct LeaveGameResponse
{
    unsigned int status = 0;
} LeaveGameResponse;