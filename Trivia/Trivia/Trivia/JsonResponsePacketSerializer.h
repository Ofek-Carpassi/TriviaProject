#pragma once
#include <vector>
#include <string>
#include "ResponseStructs.h"

typedef unsigned char Byte;
typedef std::vector<Byte> Buffer;

class JsonResponsePacketSerializer
{
public:
    static Buffer serializeLoginResponse(Response msg);
    static Buffer serializeSignUpResponse(Response msg);
    static Buffer serializeErrorResponse(Response msg);
    static Buffer serializeLogoutResponse(Response msg);
    static Buffer serializeGetRoomsResponse(GetRoomsResponse msg);
    static Buffer serializeGetPlayersInRoomResponse(GetPlayersInRoomResponse msg);
    static Buffer serializeJoinRoomResponse(Response msg);
    static Buffer serializeCreateRoomResponse(Response msg);
    static Buffer serializeHighScoreResponse(HighScoreResponse msg);
    static Buffer serializeCloseRoomResponse(Response msg);
    static Buffer serializeStartGameResponse(Response msg);
    static Buffer serializeGetRoomStateResponse(GetRoomStateResponse msg);
    static Buffer serializeLeaveRoomResponse(Response msg);
    static Buffer serializeGetGameResultsResponse(GetGameResultsResponse msg);
    static Buffer serializeSubmitAnswerResponse(SubmitAnswerResponse msg);
    static Buffer serializeGetQuestionResponse(GetQuestionResponse msg);
    static Buffer serializeLeaveGameResponse(LeaveGameResponse msg);
};