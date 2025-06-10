#include "JsonResponsePacketSerializer.h"
#include "json.hpp"  // Include the JSON library
#include "messageCodes.h"
#include "BinaryUtils.h"

using json = nlohmann::json;

Buffer JsonResponsePacketSerializer::serializeLoginResponse(Response msg) {
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;
    responseJson["message"] = msg.message;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for login response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(LOGIN_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeSignUpResponse(Response msg) {
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;
    responseJson["message"] = msg.message;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for signup response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(SIGNUP_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeErrorResponse(Response msg) {
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;
    responseJson["message"] = msg.message;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for error response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(ERROR_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeLogoutResponse(Response msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for logout response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(LOGOUT_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeGetRoomsResponse(GetRoomsResponse msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Create a JSON array for rooms
    json roomsArray = json::array();

    for (const auto& room : msg.rooms)
    {
        json roomJson;
        roomJson["id"] = room.id;
        roomJson["name"] = room.name;
        roomJson["maxPlayers"] = room.maxPlayers;
        roomJson["timePerQuestion"] = room.timePerQuestion;
        roomJson["isActive"] = room.isActive;
        roomJson["state"] = (room.state == RoomState::WAITING ? "Waiting" : "InProgress");
        roomJson["questionCount"] = 0;

        roomsArray.push_back(roomJson);
    }

    responseJson["rooms"] = roomsArray;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for get rooms response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(GET_ROOMS_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeGetPlayersInRoomResponse(GetPlayersInRoomResponse msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Create a JSON array for players
    json playersArray = json::array();

    for (const auto& player : msg.players)
    {
        playersArray.push_back(player);
    }

    responseJson["players"] = playersArray;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for get players response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(GET_PLAYERS_IN_ROOM_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeJoinRoomResponse(Response msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for join room response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(JOIN_ROOM_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeCreateRoomResponse(Response msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;
    responseJson["message"] = msg.message;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for create room response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(CREATE_ROOM_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeHighScoreResponse(HighScoreResponse msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Create a JSON array for player statistics
    json statsArray = json::array();

    for (const auto& stat : msg.statistics)
    {
        json statJson;
        statJson["username"] = stat.username;
        statJson["gamesPlayed"] = stat.gamesPlayed;
        statJson["correctAnswers"] = stat.correctAnswers;
        statJson["wrongAnswers"] = stat.wrongAnswers;
        statJson["averageAnswerTime"] = stat.averageAnswerTime;

        statsArray.push_back(statJson);
    }

    responseJson["UserStatistics"] = statsArray;

    // Create a JSON object for high scores
    json highScoresJson = json::array();

    // Sort the statistics by score (correct answers)
    std::vector<PlayerStatisticsResponse> sortedStats = msg.statistics;
    std::sort(sortedStats.begin(), sortedStats.end(),
        [](const PlayerStatisticsResponse& a, const PlayerStatisticsResponse& b) {
            return a.correctAnswers > b.correctAnswers;
        });

    // Get the top players
    int limit = std::min(static_cast<int>(sortedStats.size()), 5); // Top 5 players
    for (int i = 0; i < limit; i++)
    {
        json scoreJson;
        scoreJson["username"] = sortedStats[i].username;
        scoreJson["score"] = sortedStats[i].correctAnswers;
        highScoresJson.push_back(scoreJson);
    }

    responseJson["HighScores"] = highScoresJson;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for high score response from messageCodes.h
    ret_val.push_back(static_cast<Byte>(HIGH_SCORE_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeCloseRoomResponse(Response msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for close room response
    ret_val.push_back(static_cast<Byte>(CLOSE_ROOM_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeStartGameResponse(Response msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for start game response
    ret_val.push_back(static_cast<Byte>(START_GAME_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeGetRoomStateResponse(GetRoomStateResponse msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;
    responseJson["hasGameBegun"] = msg.hasGameBegun;

    // Create a JSON array for players
    json playersArray = json::array();
    for (const auto& player : msg.players)
    {
        playersArray.push_back(player);
    }
    responseJson["players"] = playersArray;

    responseJson["answerCount"] = msg.answerCount;
    responseJson["answerTimeout"] = msg.answerTimeout;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for get room state response
    ret_val.push_back(static_cast<Byte>(GET_ROOM_STATE_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeLeaveRoomResponse(Response msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for leave room response
    ret_val.push_back(static_cast<Byte>(LEAVE_ROOM_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeLeaveGameResponse(LeaveGameResponse msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for leave game response
    ret_val.push_back(static_cast<Byte>(LEAVE_GAME_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeSubmitAnswerResponse(SubmitAnswerResponse msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for submit answer response
    ret_val.push_back(static_cast<Byte>(SUBMIT_ANSWER_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeGetGameResultsResponse(GetGameResultsResponse msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Create a JSON array for results
    json resultsArray = json::array();

    // Only include results if status is success (1)
    if (msg.status == 1)
    {
        for (const auto& result : msg.results)
        {
            json resultJson;
            resultJson["username"] = result.username;
            resultJson["correctAnswerCount"] = result.correctAnswerCount;
            resultJson["wrongAnswerCount"] = result.wrongAnswerCount;
            resultJson["averageAnswerTime"] = result.averageAnswerTime;

            resultsArray.push_back(resultJson);
        }
    }

    responseJson["results"] = resultsArray;

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for get game results response
    ret_val.push_back(static_cast<Byte>(GET_GAME_RESULTS_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeGetQuestionResponse(GetQuestionResponse msg)
{
    // Create JSON object
    json responseJson;
    responseJson["status"] = msg.status;

    // Only include question and answers if status is success (1)
    if (msg.status == 1)
    {
        responseJson["question"] = msg.question;

        // Create a JSON object for answers
        json answersJson = json::object();
        for (const auto& pair : msg.answers)
        {
            unsigned int id = pair.first;
            std::string content = pair.second;
            // Convert the id to string since JSON uses string keys
            answersJson[std::to_string(id)] = content;
        }

        responseJson["answers"] = answersJson;
    }

    // Convert to string
    std::string jsonStr = responseJson.dump();

    // Create the binary message
    Buffer ret_val;

    // Code for get question response
    ret_val.push_back(static_cast<Byte>(GET_QUESTION_RESPONSE_CODE));

    // 4 bytes: length of JSON string
    Buffer size = ConvertToBinaryFourBytes(static_cast<int>(jsonStr.length()));
    ret_val.insert(ret_val.end(), size.begin(), size.end());

    // JSON string as raw bytes
    ret_val.insert(ret_val.end(), jsonStr.begin(), jsonStr.end());

    return ret_val;
}

Buffer JsonResponsePacketSerializer::serializeResponse(const Response& response, unsigned char messageCode)
{
    json j;
    j["status"] = response.status;
    if (!response.message.empty())
        j["message"] = response.message;

    std::string jsonStr = j.dump();

    // Create result buffer
    Buffer result;
    result.push_back(messageCode);

    // Add 4 bytes of length
    std::vector<unsigned char> lengthBytes(4);
    lengthBytes[0] = static_cast<unsigned char>((jsonStr.length() >> 24) & 0xFF);
    lengthBytes[1] = static_cast<unsigned char>((jsonStr.length() >> 16) & 0xFF);
    lengthBytes[2] = static_cast<unsigned char>((jsonStr.length() >> 8) & 0xFF);
    lengthBytes[3] = static_cast<unsigned char>(jsonStr.length() & 0xFF);

    result.insert(result.end(), lengthBytes.begin(), lengthBytes.end());

    // Add the JSON data
    result.insert(result.end(), jsonStr.begin(), jsonStr.end());

    return result;
}