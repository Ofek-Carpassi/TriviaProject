#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "RequestStructs.h"
#include "ResponseStructs.h"
#include "messageCodes.h"
#include <string>

class RequestHandlerFactory;

class GameRequestHandler : public IRequestHandler
{
public:
    GameRequestHandler(const std::string& username, unsigned int roomId, RequestHandlerFactory& handlerFactory);
    ~GameRequestHandler();

    virtual bool isRequestRelevant(const RequestInfo& request) override;
    virtual RequestResult handleRequest(const RequestInfo& request) override;

private:
    // Handler methods for each request type
    RequestResult leaveGame(const RequestInfo& request);
    RequestResult getQuestion(const RequestInfo& request);
    RequestResult submitAnswer(const RequestInfo& request);
    RequestResult getGameResults(const RequestInfo& request);

    std::string m_username;
    unsigned int m_roomId;
    RequestHandlerFactory& m_handlerFactory;
};
