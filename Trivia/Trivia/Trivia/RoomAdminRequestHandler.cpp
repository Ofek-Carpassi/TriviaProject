#include "RoomAdminRequestHandler.hpp"
#include "JsonResponsePacketSerializer.hpp"
#include <algorithm>
#include <iostream>

RoomAdminRequestHandler::RoomAdminRequestHandler(const std::string& username, unsigned int roomId, RequestHandlerFactory& handlerFactory)
    : m_username(username), m_roomId(roomId), m_handlerFactory(handlerFactory)
{
}

RoomAdminRequestHandler::~RoomAdminRequestHandler()
{
}

bool RoomAdminRequestHandler::isRequestRelevant(const RequestInfo& request)
{
    return (request.messageCode == CLOSE_ROOM_CODE ||
        request.messageCode == START_GAME_CODE ||
        request.messageCode == GET_ROOM_STATE_CODE);
}

RequestResult RoomAdminRequestHandler::handleRequest(const RequestInfo& request)
{
    switch (request.messageCode)
    {
    case CLOSE_ROOM_CODE:
        return closeRoom(request);
    case START_GAME_CODE:
        return startGame(request);
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

RequestResult RoomAdminRequestHandler::closeRoom(const RequestInfo& request)
{
    try
    {
        // Get room from room manager
        Room* room = m_handlerFactory.getRoomManager().getRoom(m_roomId);
        if (!room)
        {
            throw std::exception("Room not found");
        }

        // Verify user is the admin (first in the list)
        std::vector<std::string> users = room->getAllUsers();
        if (users.empty() || users[0] != m_username)
        {
            throw std::exception("Only the admin can close the room");
        }

        // Delete the room
        bool success = m_handlerFactory.getRoomManager().deleteRoom(m_roomId);
        if (!success)
        {
            throw std::exception("Failed to close room");
        }

        // Prepare response
        Response response;
        response.status = CLOSE_ROOM_RESPONSE_CODE;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeCloseRoomResponse(response);
        result.newHandler = m_handlerFactory.createMenuRequestHandler(m_username);
        std::cout << "Closed room" << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to close room: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult RoomAdminRequestHandler::startGame(const RequestInfo& request)
{
    try
    {
        // Get room from room manager
        Room* room = m_handlerFactory.getRoomManager().getRoom(m_roomId);
        if (!room)
        {
            throw std::exception("Room not found");
        }

        // Verify user is the admin (first in the list)
        std::vector<std::string> users = room->getAllUsers();
        if (users.empty() || users[0] != m_username)
        {
            throw std::exception("Only the admin can start the game");
        }

		// start the game
        room->setActive(true);
        room->setState(RoomState::GAME_IN_PROGRESS);

        // Get all players in the room
        std::vector<std::string> players = room->getAllUsers();

        // Initialize the game in GameManager
        unsigned int questionCount = room->getRoomData().questionCount;
        m_handlerFactory.getGameManager().createGame(m_roomId, players, questionCount);

        // Prepare response
        Response response;
        response.status = START_GAME_RESPONSE_CODE;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeStartGameResponse(response);
        // Transition to GameRequestHandler
        result.newHandler = m_handlerFactory.createGameRequestHandler(m_username, m_roomId);
        std::cout << "Game started in room " << m_roomId << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to start game: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult RoomAdminRequestHandler::getRoomState(const RequestInfo& request)
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
        response.answerCount = 0; // This would be filled from game state in a real implementation
        response.answerTimeout = room->getRoomData().timePerQuestion;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeGetRoomStateResponse(response);
        result.newHandler = this;
        std::cout << m_username << " Got room state" << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to get room state: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

