#include "RoomMemberRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include <algorithm>
#include <iostream>


RoomMemberRequestHandler::RoomMemberRequestHandler(const std::string& username, unsigned int roomId, RequestHandlerFactory& handlerFactory)
    : m_username(username), m_roomId(roomId), m_handlerFactory(handlerFactory)
{
}

RoomMemberRequestHandler::~RoomMemberRequestHandler()
{
}

bool RoomMemberRequestHandler::isRequestRelevant(const RequestInfo& request)
{
    return (request.messageCode == LEAVE_ROOM_CODE ||
        request.messageCode == GET_ROOM_STATE_CODE);
}

RequestResult RoomMemberRequestHandler::handleRequest(const RequestInfo& request)
{
    switch (request.messageCode)
    {
    case LEAVE_ROOM_CODE:
        return leaveRoom(request);
    case GET_ROOM_STATE_CODE:
        return getRoomState(request);
    default:
        Response response;
        response.status = 0;
        response.message = "Unknown request code";

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult RoomMemberRequestHandler::leaveRoom(const RequestInfo& request)
{
    try
    {
        // Get room from room manager
        Room* room = m_handlerFactory.getRoomManager().getRoom(m_roomId);
        if (!room)
        {
            throw std::exception("Room not found");
        }

        // Verify user is in the room
        std::vector<std::string> users = room->getAllUsers();
        auto it = std::find(users.begin(), users.end(), m_username);
        if (it == users.end())
        {
            throw std::exception("User is not in the room");
        }

        // Remove user from room
        if (!room->removeUser(m_username))
        {
            throw std::exception("Failed to leave room");
        }

        // Prepare response
        Response response;
        response.status = LEAVE_ROOM_RESPONSE_CODE;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeLeaveRoomResponse(response);
        result.newHandler = m_handlerFactory.createMenuRequestHandler(m_username);
        std::cout << "User leaved room!" << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to leave room: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult RoomMemberRequestHandler::getRoomState(const RequestInfo& request)
{
    try
    {
        // Get room from room manager
        Room* room = m_handlerFactory.getRoomManager().getRoom(m_roomId);
        if (!room)
        {
            throw std::exception("Room not found");
        }

        // Verify user is in the room
        std::vector<std::string> users = room->getAllUsers();
        auto it = std::find(users.begin(), users.end(), m_username);
        if (it == users.end())
        {
            throw std::exception("User is not in the room");
        }

        // Prepare response
        GetRoomStateResponse response;
        response.status = GET_ROOM_STATE_RESPONSE_CODE;
        response.hasGameBegun = room->isActive();
        response.players = users;
        response.answerCount = 0;
        response.answerTimeout = room->getRoomData().timePerQuestion;

        // Serialize response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeGetRoomStateResponse(response);

        // IMPORTANT FIX: If game has begun, switch to GameRequestHandler
        if (room->isActive())
        {
            std::cout << m_username << " transitioning to game handler via room state" << std::endl;
            result.newHandler = m_handlerFactory.createGameRequestHandler(m_username, m_roomId);
        }
        else
        {
            result.newHandler = this;
        }

        std::cout << m_username << " Got room state" << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        // Error handling (unchanged)
        Response response;
        response.status = 0;
        response.message = "Failed to get room state: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}