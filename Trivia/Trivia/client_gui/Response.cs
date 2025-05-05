using System.Collections.Generic;

namespace client_gui
{
    public abstract class Response
    {
        public int Status { get; set; }
        public string message { get; set; } = string.Empty;
    }

    public class ScoreEntry
    {
        public string Username { get; set; } = string.Empty;
        public int Score { get; set; }
    }

    public class HighScoreResponse : Response
    {
        public List<ScoreEntry> Scores { get; set; } = new List<ScoreEntry>();
    }

    public class LoginResponse : Response
    {
    }

    public class SignUpResponse : Response
    {
    }

    public class ErrorResponse : Response
    {
    }

    public class LogoutResponse : Response
    {
    }

    public class GetRoomsResponse : Response
    {
        public List<RoomS> Rooms { get; set; } = new List<RoomS>();
    }

    public class GetPlayersInRoomResponse : Response
    {
        public List<string> Players { get; set; } = new List<string>();
    }

    public class JoinRoomResponse : Response
    {
    }

    public class CreateRoomResponse : Response
    {
    }

    public class CloseRoomResponse : Response
    {
    }

    public class LeaveRoomResponse : Response
    {
    }

    public class StartGameResponse : Response
    {
    }

    public class GetRoomStateResponse : Response
    {
        public bool hasGameBegun { get; set; }
        public List<string> players { get; set; } = new List<string>();
        public int answerCount { get; set; }
        public int answerTimeout { get; set; }
    }

    public class RoomS
    {
        public int id { get; set; }
        public string name { get; set; } = string.Empty;
        public int maxPlayers { get; set; }
        public int timePerQuestion { get; set; }
        public bool isActive { get; set; }
        public string state { get; set; } = string.Empty;
    }

    public class PlayerStatistics
    {
        public string username { get; set; } = string.Empty;
        public int gamesPlayed { get; set; }
        public int correctAnswerCount { get; set; }
        public int wrongAnswerCount { get; set; }
        public float averageAnswerTime { get; set; }
    }
}