#pragma once

// Request codes (client -> server)
constexpr unsigned char LOGIN_CODE = 200;
constexpr unsigned char SIGNUP_CODE = 201;
constexpr unsigned char LOGOUT_CODE = 202;
constexpr unsigned char GET_ROOMS_CODE = 203;
constexpr unsigned char GET_PLAYERS_IN_ROOM_CODE = 204;
constexpr unsigned char JOIN_ROOM_CODE = 205;
constexpr unsigned char CREATE_ROOM_CODE = 206;
constexpr unsigned char HIGH_SCORE_CODE = 207;
constexpr unsigned char CLOSE_ROOM_CODE = 208;
constexpr unsigned char START_GAME_CODE = 209;
constexpr unsigned char GET_ROOM_STATE_CODE = 210;
constexpr unsigned char LEAVE_ROOM_CODE = 211;
constexpr unsigned char LEAVE_GAME_CODE = 212;
constexpr unsigned char GET_QUESTION_CODE = 213;
constexpr unsigned char SUBMIT_ANSWER_CODE = 214;
constexpr unsigned char GET_GAME_RESULTS_CODE = 215;

// Response codes (server -> client)
constexpr unsigned char LOGIN_RESPONSE_CODE = 100;
constexpr unsigned char SIGNUP_RESPONSE_CODE = 101;
constexpr unsigned char ERROR_RESPONSE_CODE = 102;
constexpr unsigned char LOGOUT_RESPONSE_CODE = 103;
constexpr unsigned char GET_ROOMS_RESPONSE_CODE = 104;
constexpr unsigned char GET_PLAYERS_IN_ROOM_RESPONSE_CODE = 105;
constexpr unsigned char JOIN_ROOM_RESPONSE_CODE = 106;
constexpr unsigned char CREATE_ROOM_RESPONSE_CODE = 107;
constexpr unsigned char HIGH_SCORE_RESPONSE_CODE = 108;
constexpr unsigned char CLOSE_ROOM_RESPONSE_CODE = 109;
constexpr unsigned char START_GAME_RESPONSE_CODE = 110;
constexpr unsigned char GET_ROOM_STATE_RESPONSE_CODE = 111;
constexpr unsigned char LEAVE_ROOM_RESPONSE_CODE = 112;
constexpr unsigned char GET_QUESTION_RESPONSE_CODE = 113;
constexpr unsigned char SUBMIT_ANSWER_RESPONSE_CODE = 114;
constexpr unsigned char GET_GAME_RESULTS_RESPONSE_CODE = 115;
constexpr unsigned char LEAVE_GAME_RESPONSE_CODE = 116;
