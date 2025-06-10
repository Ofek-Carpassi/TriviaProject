#include "MenuRequestHandler.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "messageCodes.h"
#include <iostream>
#include "json.hpp"
#include "BinaryUtils.h"

using json = nlohmann::json;

MenuRequestHandler::MenuRequestHandler(const std::string& username, RequestHandlerFactory& handlerFactory)
    : m_username(username), m_handlerFactory(handlerFactory)
{
}

MenuRequestHandler::~MenuRequestHandler()
{
}

// Update isRequestRelevant method to include new message codes
bool MenuRequestHandler::isRequestRelevant(const RequestInfo& request)
{
    // Check if the request code matches any of the supported actions
    return (request.messageCode == CREATE_ROOM_CODE ||
        request.messageCode == GET_ROOMS_CODE ||
        request.messageCode == GET_PLAYERS_IN_ROOM_CODE ||
        request.messageCode == JOIN_ROOM_CODE ||
        request.messageCode == HIGH_SCORE_CODE ||
        request.messageCode == LOGOUT_CODE ||
        request.messageCode == CLOSE_ROOM_CODE ||
        request.messageCode == START_GAME_CODE ||
        request.messageCode == GET_ROOM_STATE_CODE ||
        request.messageCode == LEAVE_ROOM_CODE);
}

// Update handleRequest method to route to new handler methods
RequestResult MenuRequestHandler::handleRequest(const RequestInfo& request)
{
    // Route the request to the appropriate handler based on message code
    switch (request.messageCode)
    {
    case CREATE_ROOM_CODE:
        return createRoom(request);
    case GET_ROOMS_CODE:
        return getRooms(request);
    case GET_PLAYERS_IN_ROOM_CODE:
        return getPlayersInRoom(request);
    case JOIN_ROOM_CODE:
        return joinRoom(request);
    case HIGH_SCORE_CODE:
        return getStatistics(request);
    case LOGOUT_CODE:
        return logout(request);
    case CLOSE_ROOM_CODE:
        return closeRoom(request);
    case START_GAME_CODE:
        return startGame(request);
    case GET_ROOM_STATE_CODE:
        return getRoomState(request);
    case LEAVE_ROOM_CODE:
        return leaveRoom(request);
	case ADD_QUESTION_CODE:
		return addQuestion(request);
    case JOIN_HEAD_TO_HEAD_CODE:
        return joinHeadToHead(request);
    case CHECK_HEAD_TO_HEAD_STATUS_CODE:
        return checkHeadToHeadStatus(request);
    case LEAVE_HEAD_TO_HEAD_CODE:
        return leaveHeadToHead(request);
    default:
        // Should not reach here due to isRequestRelevant check
        Response response;
        response.status = 0;
        response.message = "Unknown request code";
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::createRoom(const RequestInfo& request)
{
    try
    {
        // Deserialize request
        JsonRequestPacketDeserializer deserializer;
        CreateRoomRequest createRoomReq = deserializer.deserializeCreateRoomRequest(request.buffer);

        // Create room using RoomManager
        unsigned int roomId = m_handlerFactory.getRoomManager().createRoom(
            m_username,
            createRoomReq.roomName,
            createRoomReq.maxUsers,
            createRoomReq.answerTimeout);

        // Prepare response
        Response response;
        response.status = CREATE_ROOM_RESPONSE_CODE;
        response.message = std::to_string(roomId);

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeCreateRoomResponse(response);

        // Transition to RoomAdminRequestHandler
        result.newHandler = m_handlerFactory.createRoomAdminRequestHandler(m_username, roomId);
        std::cout << m_username << " Created room" << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to create room: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::getRooms(const RequestInfo& request)
{
    try
    {
        // Get rooms using RoomManager
        std::vector<RoomData> rooms = m_handlerFactory.getRoomManager().getRooms();

        // Prepare response
        GetRoomsResponse response;
        response.status = GET_ROOMS_RESPONSE_CODE;
        response.rooms = rooms;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeGetRoomsResponse(response);
        result.newHandler = this;
        std::cout << "Gave rooms to: " << m_username << std::endl;

        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to get rooms: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::getPlayersInRoom(const RequestInfo& request)
{
    try
    {
        // Deserialize request
        JsonRequestPacketDeserializer deserializer;
        GetPlayersInRoomRequest playersReq = deserializer.deserializeGetPlayersInRoomRequest(request.buffer);

        // Get room from RoomManager
        Room* room = m_handlerFactory.getRoomManager().getRoom(playersReq.roomId);
        if (!room)
        {
            throw std::exception("Room not found");
        }

        // Get players in room
        std::vector<std::string> players = room->getAllUsers();

        // Prepare response
        GetPlayersInRoomResponse response;
        response.status = GET_PLAYERS_IN_ROOM_RESPONSE_CODE;
        response.players = players;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeGetPlayersInRoomResponse(response);
        result.newHandler = this;
        std::cout << m_username << " got the players in room" << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to get players: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;

        return result;
    }
}

RequestResult MenuRequestHandler::joinRoom(const RequestInfo& request)
{
    try
    {
        // Deserialize request
        JsonRequestPacketDeserializer deserializer;
        JoinRoomRequest joinReq = deserializer.deserializeJoinRoomRequest(request.buffer);

        // Get room from RoomManager
        Room* room = m_handlerFactory.getRoomManager().getRoom(joinReq.roomId);
        if (!room)
        {
            throw std::exception("Room not found");
        }

        // Add player to room
        if (!room->addUser(m_username))
        {
            throw std::exception("Failed to join room");
        }

        // Prepare response
        Response response;
        response.status = JOIN_ROOM_RESPONSE_CODE;
        response.message = "Joined room successfully";

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeJoinRoomResponse(response);

        // Transition to RoomMemberRequestHandler
        result.newHandler = m_handlerFactory.createRoomMemberRequestHandler(m_username, joinReq.roomId);
        std::cout << m_username << " Joined room: " << joinReq.roomId << std::endl;

        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to join room: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::getStatistics(const RequestInfo& request)
{
    try
    {
        // Get player statistics
        PlayerStatisticsResponse playerStats = m_handlerFactory.getStatisticsManager().getPlayerStatistics(m_username);

        // Get high scores
        std::vector<PlayerStatisticsResponse> highScores = m_handlerFactory.getStatisticsManager().getHighScores();

        // Add the current player to the high scores list for completeness
        highScores.push_back(playerStats);

        // Prepare response
        HighScoreResponse response;
        response.status = 1;
        response.statistics = highScores;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeHighScoreResponse(response);
        result.newHandler = this;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to get statistics: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::logout(const RequestInfo& request)
{
    try
    {
        // Logout user
        m_handlerFactory.getLoginManager().logout(m_username);

        // Prepare response
        Response response;
        response.status = LOGOUT_RESPONSE_CODE;
        response.message = "Logged out successfully";

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeLogoutResponse(response);
        result.newHandler = m_handlerFactory.createLoginRequestHandler();
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to logout: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::closeRoom(const RequestInfo& request)
{
    try
    {
        // Get the user's room
        std::vector<RoomData> rooms = m_handlerFactory.getRoomManager().getRooms();

        // Find the room where the user is the admin
        Room* userRoom = nullptr;
        unsigned int roomId = 0;

        for (const auto& roomData : rooms)
        {
            Room* room = m_handlerFactory.getRoomManager().getRoom(roomData.id);
            if (room && room->getAllUsers().size() > 0 && room->getAllUsers()[0] == m_username)
            {
                userRoom = room;
                roomId = roomData.id;
                break;
            }
        }

        if (!userRoom)
        {
            throw std::exception("User is not an admin of any room");
        }

        // Delete the room
        bool success = m_handlerFactory.getRoomManager().deleteRoom(roomId);
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
        result.newHandler = this;
        std::cout  << m_username << " Closed room" << std::endl;

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

RequestResult MenuRequestHandler::startGame(const RequestInfo& request)
{
    try
    {
        // Get the user's room
        std::vector<RoomData> rooms = m_handlerFactory.getRoomManager().getRooms();

        // Find the room where the user is the admin
        Room* userRoom = nullptr;
        unsigned int roomId = 0;

        for (const auto& roomData : rooms)
        {
            Room* room = m_handlerFactory.getRoomManager().getRoom(roomData.id);
            if (room && room->getAllUsers().size() > 0 && room->getAllUsers()[0] == m_username)
            {
                userRoom = room;
                roomId = roomData.id;
                break;
            }
        }

        if (!userRoom)
        {
            throw std::exception("User is not an admin of any room");
        }

        // Start the game (set room to active)
        userRoom->setActive(true);
        userRoom->setState(RoomState::GAME_IN_PROGRESS);

        // Prepare response
        Response response;
        response.status = START_GAME_RESPONSE_CODE;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeStartGameResponse(response);
        result.newHandler = this;  // This might change to a GameHandler in the future
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

RequestResult MenuRequestHandler::getRoomState(const RequestInfo& request)
{
    try
    {
        // Find the room the user is in
        std::vector<RoomData> rooms = m_handlerFactory.getRoomManager().getRooms();

        Room* userRoom = nullptr;
        for (const auto& roomData : rooms)
        {
            Room* room = m_handlerFactory.getRoomManager().getRoom(roomData.id);
            if (room)
            {
                std::vector<std::string> users = room->getAllUsers();
                auto it = std::find(users.begin(), users.end(), m_username);
                if (it != users.end())
                {
                    userRoom = room;
                    break;
                }
            }
        }

        if (!userRoom)
        {
            throw std::exception("User is not in any room");
        }

        // Prepare response
        GetRoomStateResponse response;
        response.status = GET_ROOM_STATE_RESPONSE_CODE;
        response.hasGameBegun = userRoom->isActive();
        response.players = userRoom->getAllUsers();

        // These would typically come from a game state
        // For now using placeholder values
        response.answerCount = 0;
        response.answerTimeout = userRoom->getRoomData().timePerQuestion;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeGetRoomStateResponse(response);
        result.newHandler = this;
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

RequestResult MenuRequestHandler::leaveRoom(const RequestInfo& request)
{
    try
    {
        // Find the room the user is in
        std::vector<RoomData> rooms = m_handlerFactory.getRoomManager().getRooms();

        Room* userRoom = nullptr;
        unsigned int roomId = 0;

        for (const auto& roomData : rooms)
        {
            Room* room = m_handlerFactory.getRoomManager().getRoom(roomData.id);
            if (room)
            {
                std::vector<std::string> users = room->getAllUsers();
                auto it = std::find(users.begin(), users.end(), m_username);
                if (it != users.end())
                {
                    userRoom = room;
                    roomId = roomData.id;
                    break;
                }
            }
        }

        if (!userRoom)
        {
            throw std::exception("User is not in any room");
        }

        // If user is the admin (first in list), delete the room
        std::vector<std::string> users = userRoom->getAllUsers();
        if (!users.empty() && users[0] == m_username)
        {
            m_handlerFactory.getRoomManager().deleteRoom(roomId);
        }
        else
        {
            // Otherwise just remove the user
            userRoom->removeUser(m_username);
        }

        // Prepare response
        Response response;
        response.status = LEAVE_ROOM_RESPONSE_CODE;

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeLeaveRoomResponse(response);
        result.newHandler = this;
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

RequestResult MenuRequestHandler::addQuestion(const RequestInfo& request)
{
    try
    {
        // Parse the JSON
        std::string jsonString(request.buffer.begin(), request.buffer.end());
        std::cout << "Received add question request: " << jsonString << std::endl;

        // Parse JSON using the json library
        json j = json::parse(jsonString);

        // Extract question data
        std::string question = j["question"].get<std::string>();
        std::string correct_answer = j["correct_answer"].get<std::string>();

        // Extract wrong answers
        std::vector<std::string> wrong_answers;
        for (const auto& answer : j["wrong_answers"])
        {
            wrong_answers.push_back(answer.get<std::string>());
        }

        // Add to database (create this method in SqliteDatabase)
        bool success = m_handlerFactory.getDatabase()->addQuestion(question, correct_answer, wrong_answers);

        // Prepare response
        Response response;
        response.status = success ? 1 : 0;
        response.message = success ? "Question added successfully" : "Failed to add question";

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeResponse(response, ADD_QUESTION_RESPONSE_CODE);
        result.newHandler = this;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to add question: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::joinHeadToHead(const RequestInfo& request)
{
    try
    {
        // Parse JSON
        std::string jsonStr = std::string(request.buffer.begin(), request.buffer.end());
        json j = json::parse(jsonStr);
        std::string username = j["username"].get<std::string>();

        std::string opponent;
        unsigned int roomId = 0;
        bool hasMatch = m_handlerFactory.getHeadToHeadManager().joinQueue(username, opponent, roomId);

        // Create response JSON
        json responseJson;
        responseJson["status"] = 1;
        responseJson["queueSize"] = m_handlerFactory.getHeadToHeadManager().getQueueSize();
        responseJson["hasMatch"] = hasMatch;

        if (hasMatch)
        {
            responseJson["roomId"] = roomId;
            responseJson["opponent"] = opponent;
        }

        // Convert to string
        std::string jsonResponse = responseJson.dump();

        // Create binary response
        Buffer response;
        response.push_back(JOIN_HEAD_TO_HEAD_RESPONSE_CODE);

        Buffer sizeBuffer = ConvertToBinaryFourBytes(jsonResponse.size());
        response.insert(response.end(), sizeBuffer.begin(), sizeBuffer.end());
        response.insert(response.end(), jsonResponse.begin(), jsonResponse.end());

        // Return the result with the same handler
        RequestResult result;
        result.response = response;
        result.newHandler = this;

        if (hasMatch)
        {
            // If matched, switch to game request handler
            result.newHandler = m_handlerFactory.createGameRequestHandler(username, roomId);
        }

        return result;
    }
    catch (std::exception& e)
    {
        // Return error
        Response errorResponse;
        errorResponse.status = 0;
        errorResponse.message = e.what();

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(errorResponse);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::checkHeadToHeadStatus(const RequestInfo& request)
{
    try
    {
        // Parse JSON
        std::string jsonStr = std::string(request.buffer.begin(), request.buffer.end());
        json j = json::parse(jsonStr);
        std::string username = j["username"].get<std::string>();

        std::string opponent;
        unsigned int roomId = 0;
        bool hasMatch = m_handlerFactory.getHeadToHeadManager().checkMatch(username, opponent, roomId);

        // Create response JSON
        json responseJson;
        responseJson["status"] = 1;
        responseJson["queueSize"] = m_handlerFactory.getHeadToHeadManager().getQueueSize();
        responseJson["hasMatch"] = hasMatch;

        if (hasMatch)
        {
            responseJson["roomId"] = roomId;
            responseJson["opponent"] = opponent;
        }

        // Convert to string
        std::string jsonResponse = responseJson.dump();

        // Create binary response
        Buffer response;
        response.push_back(CHECK_HEAD_TO_HEAD_STATUS_RESPONSE_CODE);

        Buffer sizeBuffer = ConvertToBinaryFourBytes(jsonResponse.size());
        response.insert(response.end(), sizeBuffer.begin(), sizeBuffer.end());
        response.insert(response.end(), jsonResponse.begin(), jsonResponse.end());

        // Return the result with the same handler
        RequestResult result;
        result.response = response;
        result.newHandler = this;

        if (hasMatch)
        {
            // If matched, switch to game request handler
            result.newHandler = m_handlerFactory.createGameRequestHandler(username, roomId);
        }

        return result;
    }
    catch (std::exception& e)
    {
        // Return error
        Response errorResponse;
        errorResponse.status = 0;
        errorResponse.message = e.what();

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(errorResponse);
        result.newHandler = this;
        return result;
    }
}

RequestResult MenuRequestHandler::leaveHeadToHead(const RequestInfo& request)
{
    try
    {
        // Parse JSON
        std::string jsonStr = std::string(request.buffer.begin(), request.buffer.end());
        json j = json::parse(jsonStr);
        std::string username = j["username"].get<std::string>();

        // Remove from queue
        m_handlerFactory.getHeadToHeadManager().leaveQueue(username);

        // Create response JSON
        json responseJson;
        responseJson["status"] = 1;

        // Convert to string
        std::string jsonResponse = responseJson.dump();

        // Create binary response
        Buffer response;
        response.push_back(LEAVE_HEAD_TO_HEAD_RESPONSE_CODE);

        Buffer sizeBuffer = ConvertToBinaryFourBytes(jsonResponse.size());
        response.insert(response.end(), sizeBuffer.begin(), sizeBuffer.end());
        response.insert(response.end(), jsonResponse.begin(), jsonResponse.end());

        // Return the result
        RequestResult result;
        result.response = response;
        result.newHandler = this;
        return result;
    }
    catch (std::exception& e)
    {
        // Return error
        Response errorResponse;
        errorResponse.status = 0;
        errorResponse.message = e.what();

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(errorResponse);
        result.newHandler = this;
        return result;
    }
}