namespace client_gui
{
    public static class MessageCodes
    {
        // Request codes (client -> server)
        public const byte LOGIN_CODE = 200;
        public const byte SIGNUP_CODE = 201;
        public const byte LOGOUT_CODE = 202;
        public const byte GET_ROOMS_CODE = 203;
        public const byte GET_PLAYERS_IN_ROOM_CODE = 204;
        public const byte JOIN_ROOM_CODE = 205;
        public const byte CREATE_ROOM_CODE = 206;
        public const byte HIGH_SCORE_CODE = 207;
        public const byte CLOSE_ROOM_CODE = 208;
        public const byte START_GAME_CODE = 209;
        public const byte GET_ROOM_STATE_CODE = 210;
        public const byte LEAVE_ROOM_CODE = 211;
        public const byte GET_QUESTION_CODE = 213;
        public const byte SUBMIT_ANSWER_CODE = 214;
        public const byte GET_GAME_RESULTS_CODE = 215;
        public const byte LEAVE_GAME_CODE = 216;
        public const byte ADD_QUESTION_CODE = 220;
        public const byte JOIN_HEAD_TO_HEAD_CODE = 230;
        public const byte CHECK_HEAD_TO_HEAD_STATUS_CODE = 231;
        public const byte LEAVE_HEAD_TO_HEAD_CODE = 232;

        // Response codes (server -> client)
        public const byte LOGIN_RESPONSE_CODE = 100;
        public const byte SIGNUP_RESPONSE_CODE = 101;
        public const byte ERROR_RESPONSE_CODE = 102;
        public const byte LOGOUT_RESPONSE_CODE = 103;
        public const byte GET_ROOMS_RESPONSE_CODE = 104;
        public const byte GET_PLAYERS_IN_ROOM_RESPONSE_CODE = 105;
        public const byte JOIN_ROOM_RESPONSE_CODE = 106;
        public const byte CREATE_ROOM_RESPONSE_CODE = 107;
        public const byte HIGH_SCORE_RESPONSE_CODE = 108;
        public const byte CLOSE_ROOM_RESPONSE_CODE = 109;
        public const byte START_GAME_RESPONSE_CODE = 110;
        public const byte GET_ROOM_STATE_RESPONSE_CODE = 111;
        public const byte LEAVE_ROOM_RESPONSE_CODE = 112;
        public const byte GET_QUESTION_RESPONSE_CODE = 113;
        public const byte SUBMIT_ANSWER_RESPONSE_CODE = 114;
        public const byte GET_GAME_RESULTS_RESPONSE_CODE = 115;
        public const byte LEAVE_GAME_RESPONSE_CODE = 116;
        public const byte ADD_QUESTION_RESPONSE_CODE = 120;
        public const byte JOIN_HEAD_TO_HEAD_RESPONSE_CODE = 130;
        public const byte CHECK_HEAD_TO_HEAD_STATUS_RESPONSE_CODE = 131;
        public const byte LEAVE_HEAD_TO_HEAD_RESPONSE_CODE = 132;
    }
}
