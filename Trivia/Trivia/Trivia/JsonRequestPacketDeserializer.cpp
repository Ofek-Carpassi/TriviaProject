#include "JsonRequestPacketDeserializer.hpp"
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

std::string JsonRequestPacketDeserializer::bufferToString(const Buffer& buffer)
{
    // Create a string from the buffer ensuring null-termination and proper conversion
    std::string result(buffer.begin(), buffer.end());

    // Debug the received JSON
    std::cout << "Received JSON: " << result << std::endl;

    return result;
}

LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const Buffer& buffer)
{
    try {
        std::string jsonString = bufferToString(buffer);
        json j = json::parse(jsonString);

        LoginRequest req;
        req.username = j["username"].get<std::string>();
        req.password = j["password"].get<std::string>();

        return req;
    }
    catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        throw std::exception(("JSON parsing error: " + std::string(e.what())).c_str());
    }
}

SignUpRequest JsonRequestPacketDeserializer::deserializeSignUpRequest(const std::vector<uint8_t>& buffer)
{
    try {
        std::string jsonString = bufferToString(buffer);
        json j = json::parse(jsonString);

        SignUpRequest req;
        req.username = j["username"].get<std::string>();
        req.password = j["password"].get<std::string>();
        req.email = j["email"].get<std::string>();
        req.street = j["street"].get<std::string>();
        req.aptNumber = j["aptNumber"].get<std::string>();
        req.city = j["city"].get<std::string>();
        req.phone = j["phone"].get<std::string>();
        req.birthDate = j["birthDate"].get<std::string>();

        return req;
    }
    catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        throw std::exception(("JSON parsing error: " + std::string(e.what())).c_str());
    }
}

GetPlayersInRoomRequest JsonRequestPacketDeserializer::deserializeGetPlayersInRoomRequest(Buffer msg)
{                  
    std::string jsonString = bufferToString(msg);
    json j = json::parse(jsonString);

    GetPlayersInRoomRequest ret_val;
    ret_val.roomId = j["roomId"].get<unsigned int>(); 
    return ret_val;
}

JoinRoomRequest JsonRequestPacketDeserializer::deserializeJoinRoomRequest(Buffer msg)
{
    std::string jsonString = bufferToString(msg);
    json j = json::parse(jsonString);

    JoinRoomRequest ret_val;
    ret_val.roomId = j["roomId"].get<unsigned int>();
    return ret_val;
}

CreateRoomRequest JsonRequestPacketDeserializer::deserializeCreateRoomRequest(Buffer msg)
{		
    std::string jsonString = bufferToString(msg);
    json j = json::parse(jsonString);
    
    CreateRoomRequest ret_val;
    ret_val.roomName = j["name"].get<std::string>();
    ret_val.maxUsers = j["maxPlayers"].get<unsigned int>();
    ret_val.questionCount = j["questionCount"].get<unsigned int>();  // Changed from "playerCount" to "questionCount"
    ret_val.answerTimeout = j["timePerQuestion"].get<unsigned int>();
    return ret_val;
}

SubmitAnswerRequest JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(const Buffer& buffer)
{
    // Convert binary data to string
    std::string jsonString(buffer.begin(), buffer.end());

    // Parse JSON
    json data = json::parse(jsonString);

    // Create and populate request object
    SubmitAnswerRequest request;
    request.answerId = data["answerId"].get<unsigned int>();
    request.answerTime = data["answerTime"].get<double>();

    return request;
}

LeaveGameRequest JsonRequestPacketDeserializer::deserializeLeaveGameRequest(const Buffer& buffer)
{
    // Leave game requests don't need any additional data
    // since a user can only be in one game at a time
    return LeaveGameRequest();
}

GetQuestionRequest JsonRequestPacketDeserializer::deserializeGetQuestionRequest(const Buffer& buffer)
{
    // Get question requests don't need any additional data
    // The server knows which game the user is in
    return GetQuestionRequest();
}

GetGameResultsRequest JsonRequestPacketDeserializer::deserializeGetGameResultsRequest(const Buffer& buffer)
{
    // Get game results requests don't need any additional data 
    // The server knows which game the user is in
    return GetGameResultsRequest();
}

