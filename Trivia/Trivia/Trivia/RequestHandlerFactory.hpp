#pragma once
#include "IRequestHandler.hpp"
#include "LoginRequestHandler.hpp"
#include "MenuRequestHandler.hpp"
#include "RoomAdminRequestHandler.hpp"
#include "RoomMemberRequestHandler.hpp"
#include "GameRequestHandler.hpp"
#include "SqliteDatabase.hpp"
#include "LoginManager.hpp"
#include "RoomManager.hpp"
#include "StatisticsManager.hpp"
#include "GameManager.hpp"
#include <string>

class LoginRequestHandler;
class MenuRequestHandler;
class RoomAdminRequestHandler;
class RoomMemberRequestHandler;
class GameRequestHandler;

class RequestHandlerFactory
{
public:
    RequestHandlerFactory(IDatabase* database);
    ~RequestHandlerFactory();

    LoginRequestHandler* createLoginRequestHandler();
    MenuRequestHandler* createMenuRequestHandler(const std::string& username);
    RoomAdminRequestHandler* createRoomAdminRequestHandler(const std::string& username, unsigned int roomId);
    RoomMemberRequestHandler* createRoomMemberRequestHandler(const std::string& username, unsigned int roomId);
    GameRequestHandler* createGameRequestHandler(const std::string& username, unsigned int roomId);

    LoginManager& getLoginManager();
    RoomManager& getRoomManager();
    StatisticsManager& getStatisticsManager();
    GameManager& getGameManager();

    IDatabase* getDatabase() const;

private:
    IDatabase* m_database;
    LoginManager m_loginManager;
    RoomManager m_roomManager;
    StatisticsManager m_statisticsManager;
    GameManager m_gameManager;
};

