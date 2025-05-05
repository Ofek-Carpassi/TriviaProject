#include "LoginRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "RequestStructs.h"
#include "ResponseStructs.h"
#include <exception>

LoginRequestHandler::LoginRequestHandler(LoginManager& loginManager, RequestHandlerFactory& handlerFactory) :
    m_loginManager(loginManager), m_handlerFactory(handlerFactory)
{
}

LoginRequestHandler::~LoginRequestHandler()
{
}

bool LoginRequestHandler::isRequestRelevant(const RequestInfo& request)
{
    // 200 = Login, 201 = SignUp
    return (request.messageCode == 200 || request.messageCode == 201);
}

RequestResult LoginRequestHandler::handleRequest(const RequestInfo& request)
{
    if (request.messageCode == 200) // login
        return login(request);
    else if (request.messageCode == 201) // signup
        return signup(request);

    Response response;
    response.status = 102; // Error
    response.message = "Error: Request not valid for LoginRequestHandler";

    RequestResult result;
    result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
    result.newHandler = this; // Stay with the same handler

    return result;
}

RequestResult LoginRequestHandler::login(const RequestInfo& request)
{
    try
    {
        JsonRequestPacketDeserializer deserializer;
        // Deserialize the login request
        LoginRequest loginReq = deserializer.deserializeLoginRequest(request.buffer);

        // Attempt to login using the login manager
        int loginResult = m_loginManager.login(loginReq.username, loginReq.password);

        if (loginResult == 0) {
            throw std::exception("Login failed: Invalid username or password");
        }

        Response response;
        response.status = 100; // Success
        response.message = "Login successful";

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeLoginResponse(response);
        // On successful login, transition to the menu handler
        result.newHandler = m_handlerFactory.createMenuRequestHandler(loginReq.username);

        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 102; // Error
        response.message = "Error: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this; // Stay with the same handler on error

        return result;
    }
}

RequestResult LoginRequestHandler::signup(const RequestInfo& request)
{
    try
    {
        JsonRequestPacketDeserializer deserializer;
        // Deserialize the signup request
        SignUpRequest signupReq = deserializer.deserializeSignUpRequest(request.buffer);

        // Attempt to signup using the login manager with all the new fields
        int signupResult = m_loginManager.signup(
            signupReq.username,
            signupReq.password,
            signupReq.email,
            signupReq.street,
            signupReq.aptNumber,
            signupReq.city,
            signupReq.phone,
            signupReq.birthDate);

        if (signupResult == 0) {
            throw std::exception("Signup failed: Username may already exist or validation failed");
        }

        Response response;
        response.status = 101; // Success
        response.message = "Signup successful";

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeSignUpResponse(response);
        // On successful login, transition to the menu handler
        result.newHandler = m_handlerFactory.createMenuRequestHandler(signupReq.username);

        return result;
    }
    catch (const std::exception& e)
    {
        Response response;
        response.status = 102; // Error
        response.message = "Error: " + std::string(e.what());

        RequestResult result;
        result.response = JsonResponsePacketSerializer::serializeErrorResponse(response);
        result.newHandler = this; // Stay with the same handler on error

        return result;
    }
}