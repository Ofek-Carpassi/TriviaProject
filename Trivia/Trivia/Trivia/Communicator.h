#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <thread>
#include <map>
#include <mutex>
#include "IRequestHandler.h"
#include "RequestInfo.h"
#include "Helper.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"

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
    RequestInfo buildRequestInfo(Buffer& buffer);

    std::map<SOCKET, IRequestHandler*> m_clients;
    RequestHandlerFactory& m_handlerFactory;
    SOCKET m_serverSocket;
    std::mutex m_mutex;
    bool m_running;
};