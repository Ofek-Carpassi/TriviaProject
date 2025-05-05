#pragma once

#include <WinSock2.hpp>
#include <Windows.hpp>
#include <thread>
#include <map>
#include <mutex>
#include "IRequestHandler.hpp"
#include "RequestInfo.hpp"
#include "Helper.hpp"
#include "JsonRequestPacketDeserializer.hpp"
#include "JsonResponsePacketSerializer.hpp"

class RequestHandlerFactory;

class Communicator
{
public:
    Communicator(RequestHandlerFactory& handlerFactory);
    ~Communicator();

    void startHandleRequests();
    void close();

private:
    void bindAndListen();
    void handleNewClient(SOCKET clientSocket);
    void clientHandler(SOCKET clientSocket);

    // binary protocol
    Buffer getMessageFromClient(SOCKET clientSocket);
    void sendMessageToClient(SOCKET clientSocket, const Buffer& buffer);
    RequestInfo buildRequestInfo(const Buffer& buffer);

    std::map<SOCKET, IRequestHandler*> m_clients;
    RequestHandlerFactory& m_handlerFactory;
    SOCKET m_serverSocket;
    std::mutex m_mutex;
    bool m_running;
};

