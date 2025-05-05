#pragma once
#include "IRequestHandler.hpp"
#include "LoginManager.hpp"
#include "RequestHandlerFactory.hpp"

class RequestHandlerFactory;

class LoginRequestHandler : public IRequestHandler
{
public:
    LoginRequestHandler(LoginManager& loginManager, RequestHandlerFactory& handlerFactory);
    ~LoginRequestHandler();

    virtual bool isRequestRelevant(const RequestInfo& request) override;
    virtual RequestResult handleRequest(const RequestInfo& request) override;

private:
    RequestResult login(const RequestInfo& request);
    RequestResult signup(const RequestInfo& request);

    LoginManager& m_loginManager;
    RequestHandlerFactory& m_handlerFactory;
};

