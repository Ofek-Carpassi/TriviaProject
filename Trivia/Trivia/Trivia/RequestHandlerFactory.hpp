#pragma once
#include "IRequestHandler.h"
#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "GameRequestHandler.h"
#include "SqliteDatabase.h"
#include "LoginManager.h"
#include "RoomManager.h"
#include "StatisticsManager.h"
#include "GameManager.h"
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
