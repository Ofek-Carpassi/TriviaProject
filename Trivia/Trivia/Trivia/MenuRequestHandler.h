#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "RequestStructs.h"
#include "ResponseStructs.h"
#include "messageCodes.h"
#include <string>

class RequestHandlerFactory;

class MenuRequestHandler : public IRequestHandler
{
public:
    MenuRequestHandler(const std::string& username, RequestHandlerFactory& handlerFactory);
    ~MenuRequestHandler();

    virtual bool isRequestRelevant(const RequestInfo& request) override;
    virtual RequestResult handleRequest(const RequestInfo& request) override;

private:
    // Handler methods for each request type
    RequestResult createRoom(const RequestInfo& request);
    RequestResult getRooms(const RequestInfo& request);
    RequestResult getPlayersInRoom(const RequestInfo& request);
    RequestResult joinRoom(const RequestInfo& request);
    RequestResult getStatistics(const RequestInfo& request);
    RequestResult logout(const RequestInfo& request);
    RequestResult closeRoom(const RequestInfo& request);
    RequestResult startGame(const RequestInfo& request);
    RequestResult getRoomState(const RequestInfo& request);
    RequestResult leaveRoom(const RequestInfo& request);

    std::string m_username;
    RequestHandlerFactory& m_handlerFactory;
};