#pragma once
#include <vector>
#include <string>

typedef struct LoginRequest
{
	std::string username;
	std::string password;
} LoginRequest;

typedef struct SignUpRequest
{
    std::string username;
    std::string password;
    std::string email;
    std::string street;
    std::string aptNumber;
    std::string city;
    std::string phone;
    std::string birthDate;
} SignUpRequest;

typedef struct GetPlayersInRoomRequest
{
    unsigned int roomId;
} GetPlayersInRoomRequest;

typedef struct CreateRoomRequest
{
    std::string roomName;
    unsigned int maxUsers;
    unsigned int questionCount;
    unsigned int answerTimeout;
} CreateRoomRequest;

typedef struct JoinRoomRequest
{
    unsigned int roomId;
} JoinRoomRequest;

struct SubmitAnswerRequest {
    unsigned int answerId;
    double answerTime;
};

struct LeaveGameRequest {};
struct GetQuestionRequest {};
struct GetGameResultsRequest {};
