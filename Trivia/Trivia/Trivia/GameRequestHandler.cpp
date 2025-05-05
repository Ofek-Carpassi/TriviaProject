#include "GameRequestHandler.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include <algorithm>
#include <iostream>

GameRequestHandler::GameRequestHandler(const std::string& username, unsigned int roomId, RequestHandlerFactory& handlerFactory)
    : m_username(username), m_roomId(roomId), m_handlerFactory(handlerFactory)
{
}

GameRequestHandler::~GameRequestHandler()
{
}

bool GameRequestHandler::isRequestRelevant(const RequestInfo& request)
{
    return (request.messageCode == LEAVE_GAME_CODE ||
        request.messageCode == GET_QUESTION_CODE ||
        request.messageCode == SUBMIT_ANSWER_CODE ||
        request.messageCode == GET_GAME_RESULTS_CODE);
}

RequestResult GameRequestHandler::handleRequest(const RequestInfo& request)
{
    if (request.messageCode == LEAVE_GAME_CODE)
        return leaveGame(request);
    else if (request.messageCode == GET_QUESTION_CODE)
        return getQuestion(request);
    else if (request.messageCode == SUBMIT_ANSWER_CODE)
        return submitAnswer(request);
    else if (request.messageCode == GET_GAME_RESULTS_CODE)
        return getGameResults(request);
    else
    {
        Response response;
        response.status = 0;
        response.message = "Unknown request code";

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult GameRequestHandler::leaveGame(const RequestInfo& request)
{
    try
    {
        // Deserialize the leave game request
        LeaveGameRequest leaveGameReq = JsonRequestPacketDeserializer::deserializeLeaveGameRequest(request.buffer);

        // Tell the RoomManager to remove the user from the room
        Room* room = m_handlerFactory.getRoomManager().getRoom(m_roomId);
        if (room)
        {
            room->removeUser(m_username);
        }

        // Tell the GameManager the player left
        m_handlerFactory.getGameManager().playerLeftGame(m_username);

        // Prepare response
        LeaveGameResponse response;
        response.status = 1; // Success

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeLeaveGameResponse(response);
        result.newHandler = m_handlerFactory.createMenuRequestHandler(m_username);
        std::cout << m_username << " left game in room " << m_roomId << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to leave game: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult GameRequestHandler::getQuestion(const RequestInfo& request)
{
    try
    {
        // Deserialize the get question request
        GetQuestionRequest getQuestionReq = JsonRequestPacketDeserializer::deserializeGetQuestionRequest(request.buffer);

        // Get the next question from GameManager
        GetQuestionResponse response = m_handlerFactory.getGameManager().getQuestion(m_username);

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeGetQuestionResponse(response);
        result.newHandler = this;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to get question: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult GameRequestHandler::submitAnswer(const RequestInfo& request)
{
    try
    {
        // Deserialize the submit answer request
        SubmitAnswerRequest submitAnswerReq = JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(request.buffer);

        // Submit the answer to GameManager
        bool isCorrect = m_handlerFactory.getGameManager().submitAnswer(
            m_username, submitAnswerReq.answerId, submitAnswerReq.answerTime);

        // Prepare response
        SubmitAnswerResponse response;
        response.status = 1; // Success

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeSubmitAnswerResponse(response);
        result.newHandler = this;
        std::cout << m_username << " submitted answer " << (isCorrect ? "(correct)" : "(incorrect)") << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to submit answer: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}

RequestResult GameRequestHandler::getGameResults(const RequestInfo& request)
{
    try
    {
        // Deserialize the get game results request
        GetGameResultsRequest getResultsReq = JsonRequestPacketDeserializer::deserializeGetGameResultsRequest(request.buffer);

        // Get game results from GameManager
        GetGameResultsResponse response = m_handlerFactory.getGameManager().getGameResults(m_roomId);

        // Serialize and return response
        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeGetGameResultsResponse(response);
        result.newHandler = this;
        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 0;
        response.message = "Failed to get game results: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this;
        return result;
    }
}
