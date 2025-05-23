#pragma once
#include "IRequestHandler.hpp"
#include "RequestHandlerFactory.hpp"
#include "RequestStructs.hpp"
#include "ResponseStructs.hpp"
#include "messageCodes.hpp"
#include <string>

class RequestHandlerFactory;

class RoomMemberRequestHandler : public IRequestHandler
{
public:
    RoomMemberRequestHandler(const std::string& username, unsigned int roomId, RequestHandlerFactory& handlerFactory);
    ~RoomMemberRequestHandler();

    virtual bool isRequestRelevant(const RequestInfo& request) override;
    virtual RequestResult handleRequest(const RequestInfo& request) override;

private:
    // Handler methods for each request type
    RequestResult leaveRoom(const RequestInfo& request);
    RequestResult getRoomState(const RequestInfo& request);

    std::string m_username;
    unsigned int m_roomId;
    RequestHandlerFactory& m_handlerFactory;
};

