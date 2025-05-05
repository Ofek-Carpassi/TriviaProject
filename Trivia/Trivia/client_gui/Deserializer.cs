using Newtonsoft.Json;
using System;

namespace client_gui
{
    internal static class Deserializer
    {
        public static Response DeserializeResponse((byte status, string json) data)
        {
            try
            {
                Console.WriteLine($"Deserializing response: Status={data.status}, JSON={data.json}");

                if (string.IsNullOrEmpty(data.json))
                {
                    Console.WriteLine("Warning: Empty JSON in response");
                    return new ErrorResponse
                    {
                        Status = data.status,
                        message = "Empty response from server"
                    };
                }

                // Deserialize based on status code
                Response? response = data.status switch
                {
                    MessageCodes.LOGIN_RESPONSE_CODE => JsonConvert.DeserializeObject<LoginResponse>(data.json),
                    MessageCodes.SIGNUP_RESPONSE_CODE => JsonConvert.DeserializeObject<SignUpResponse>(data.json),
                    MessageCodes.ERROR_RESPONSE_CODE => JsonConvert.DeserializeObject<ErrorResponse>(data.json),
                    MessageCodes.LOGOUT_RESPONSE_CODE => JsonConvert.DeserializeObject<LogoutResponse>(data.json),
                    MessageCodes.GET_ROOMS_RESPONSE_CODE => JsonConvert.DeserializeObject<GetRoomsResponse>(data.json),
                    MessageCodes.GET_PLAYERS_IN_ROOM_RESPONSE_CODE => JsonConvert.DeserializeObject<GetPlayersInRoomResponse>(data.json),
                    MessageCodes.JOIN_ROOM_RESPONSE_CODE => JsonConvert.DeserializeObject<JoinRoomResponse>(data.json),
                    MessageCodes.CREATE_ROOM_RESPONSE_CODE => JsonConvert.DeserializeObject<CreateRoomResponse>(data.json),
                    MessageCodes.HIGH_SCORE_RESPONSE_CODE => JsonConvert.DeserializeObject<HighScoreResponse>(data.json),
                    MessageCodes.CLOSE_ROOM_RESPONSE_CODE => JsonConvert.DeserializeObject<CloseRoomResponse>(data.json),
                    MessageCodes.LEAVE_ROOM_RESPONSE_CODE => JsonConvert.DeserializeObject<LeaveRoomResponse>(data.json),
                    MessageCodes.START_GAME_RESPONSE_CODE => JsonConvert.DeserializeObject<StartGameResponse>(data.json),
                    MessageCodes.GET_ROOM_STATE_RESPONSE_CODE => JsonConvert.DeserializeObject<GetRoomStateResponse>(data.json),
                    _ => JsonConvert.DeserializeObject<ErrorResponse>($"{{\"Status\": {data.status}, \"message\": \"Unknown response code: {data.status}\"}}")
                };

                // Set the status if deserialization returns null
                if (response == null)
                {
                    Console.WriteLine($"Warning: Deserialization returned null for code {data.status}");
                    return new ErrorResponse
                    {
                        Status = data.status,
                        message = $"Failed to parse server response for code {data.status}"
                    };
                }

                // Set status from the received status byte if it wasn't set during deserialization
                if (response.Status == 0)
                {
                    response.Status = data.status;
                }

                return response;
            }
            catch (JsonException ex)
            {
                Console.WriteLine($"JSON deserialization error: {ex.Message}");
                return new ErrorResponse
                {
                    Status = MessageCodes.ERROR_RESPONSE_CODE,
                    message = $"JSON parsing error: {ex.Message}"
                };
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected deserialization error: {ex.Message}");
                return new ErrorResponse
                {
                    Status = MessageCodes.ERROR_RESPONSE_CODE,
                    message = $"Error processing response: {ex.Message}"
                };
            }
        }
    }
}