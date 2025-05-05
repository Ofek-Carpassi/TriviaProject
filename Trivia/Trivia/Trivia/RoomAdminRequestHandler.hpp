#pragma once
#include "IRequestHandler.hpp"
#include "RequestHandlerFactory.hpp"
#include "RequestStructs.hpp"
#include "ResponseStructs.hpp"
#include "messageCodes.hpp"
#include <string>

class RequestHandlerFactory;

class RoomAdminRequestHandler : public IRequestHandler
{
public:
    RoomAdminRequestHandler(const std::string& username, unsigned int roomId, RequestHandlerFactory& handlerFactory);
    ~RoomAdminRequestHandler();

    virtual bool isRequestRelevant(const RequestInfo& request) override;
    virtual RequestResult handleRequest(const RequestInfo& request) override;

private:
    // Handler methods for each request type
    RequestResult closeRoom(const RequestInfo& request);
    RequestResult startGame(const RequestInfo& request);
    RequestResult getRoomState(const RequestInfo& request);

    std::string m_username;
    unsigned int m_roomId;
    RequestHandlerFactory& m_handlerFactory;
};

