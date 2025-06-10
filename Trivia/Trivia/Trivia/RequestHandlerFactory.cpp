#include "RequestHandlerFactory.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"

RequestHandlerFactory::RequestHandlerFactory(IDatabase* database) :
    m_database(database),
    m_loginManager(database),
    m_statisticsManager(database),
    m_gameManager(database),
    m_headToHeadManager(&m_gameManager, &m_roomManager)
{
}

RequestHandlerFactory::~RequestHandlerFactory()
{
}

LoginRequestHandler* RequestHandlerFactory::createLoginRequestHandler()
{
    return new LoginRequestHandler(m_loginManager, *this);
}

MenuRequestHandler* RequestHandlerFactory::createMenuRequestHandler(const std::string& username)
{
    return new MenuRequestHandler(username, *this);
}

RoomAdminRequestHandler* RequestHandlerFactory::createRoomAdminRequestHandler(const std::string& username, unsigned int roomId)
{
    return new RoomAdminRequestHandler(username, roomId, *this);
}

RoomMemberRequestHandler* RequestHandlerFactory::createRoomMemberRequestHandler(const std::string& username, unsigned int roomId)
{
    return new RoomMemberRequestHandler(username, roomId, *this);
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
    return m_loginManager;
}

RoomManager& RequestHandlerFactory::getRoomManager()
{
    return m_roomManager;
}

StatisticsManager& RequestHandlerFactory::getStatisticsManager()
{
    return m_statisticsManager;
}

IDatabase* RequestHandlerFactory::getDatabase() const
{
	return m_database;
}

GameRequestHandler* RequestHandlerFactory::createGameRequestHandler(const std::string& username, unsigned int roomId)
{
    return new GameRequestHandler(username, roomId, *this);
}

GameManager& RequestHandlerFactory::getGameManager()
{
    return m_gameManager;
}

HeadToHeadManager& RequestHandlerFactory::getHeadToHeadManager()
{
    return m_headToHeadManager;
}