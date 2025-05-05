#pragma once

#include "RequestInfo.hpp"
#include "ResponseStructs.hpp"
#include "RequestStructs.hpp"

class JsonRequestPacketDeserializer
{
public:
	JsonRequestPacketDeserializer() = default;
	~JsonRequestPacketDeserializer() = default;

	static LoginRequest deserializeLoginRequest(const Buffer& buffer);
	static SignUpRequest deserializeSignUpRequest(const Buffer& buffer);
	static GetPlayersInRoomRequest deserializeGetPlayersInRoomRequest(Buffer msg);
	static JoinRoomRequest deserializeJoinRoomRequest(Buffer msg);
	static CreateRoomRequest deserializeCreateRoomRequest(Buffer msg);	 
	static SubmitAnswerRequest deserializeSubmitAnswerRequest(const Buffer& buffer);
	static LeaveGameRequest deserializeLeaveGameRequest(const Buffer& buffer);
	static GetQuestionRequest deserializeGetQuestionRequest(const Buffer& buffer);
	static GetGameResultsRequest deserializeGetGameResultsRequest(const Buffer& buffer);


private:
	static std::string bufferToString(const Buffer& buffer);
};

