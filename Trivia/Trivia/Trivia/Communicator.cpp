#include "Communicator.h"
#include "RequestHandlerFactory.h"
#include "Helper.h"
#include <iostream>
#include <string>

Communicator::Communicator(RequestHandlerFactory& handlerFactory) :
    m_handlerFactory(handlerFactory), m_serverSocket(INVALID_SOCKET), m_running(false)
{
}

Communicator::~Communicator()
{
    close();
}

void Communicator::startHandleRequests()
{
    m_running = true;
    bindAndListen();

    while (m_running)
    {
        // Accept client connections
        SOCKET clientSocket = accept(m_serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            if (!m_running) break;

            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Create a thread to handle the client
        std::thread clientThread(&Communicator::handleNewClient, this, clientSocket);
        clientThread.detach();
    }
}

void Communicator::bindAndListen()
{
    m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_serverSocket == INVALID_SOCKET)
        throw std::exception("Error creating socket");

    sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = INADDR_ANY;
    serverService.sin_port = htons(8826); // Using port 8826

    if (bind(m_serverSocket, (SOCKADDR*)&serverService, sizeof(serverService)) == SOCKET_ERROR)
    {
        closesocket(m_serverSocket);
        throw std::exception("Error binding socket");
    }

    if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(m_serverSocket);
        throw std::exception("Error listening on socket");
    }

    std::cout << "Server is listening on port 8826" << std::endl;
}

Buffer Communicator::getMessageFromClient(SOCKET clientSocket)
{
    // read message code
    Byte code;
    int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&code), 1, 0);
    if (bytesReceived <= 0)
        throw std::exception("Error receiving message code");

    // read message length
    int length = 0;
    char lengthBuffer[4];
    bytesReceived = recv(clientSocket, lengthBuffer, 4, 0);
    if (bytesReceived != 4)
        throw std::exception("Error receiving message length");

    // convert length buffer to int
    length = (unsigned char)lengthBuffer[0] << 24 |
        (unsigned char)lengthBuffer[1] << 16 |
        (unsigned char)lengthBuffer[2] << 8 |
        (unsigned char)lengthBuffer[3];

    // read message data
    Buffer buffer(length);
    if (length > 0)
    {
        bytesReceived = recv(clientSocket, reinterpret_cast<char*>(buffer.data()), length, 0);
        if (bytesReceived != length)
            throw std::exception("Error receiving message data");
    }

    // create the complete message with code and data
    Buffer message;
    message.push_back(code);

    // add the length bytes
    message.push_back(lengthBuffer[0]);
    message.push_back(lengthBuffer[1]);
    message.push_back(lengthBuffer[2]);
    message.push_back(lengthBuffer[3]);

    // add the content
    message.insert(message.end(), buffer.begin(), buffer.end());

    return message;
}

void Communicator::sendMessageToClient(SOCKET clientSocket, const Buffer& buffer)
{
    int bytesSent = send(clientSocket, reinterpret_cast<const char*>(buffer.data()), buffer.size(), 0);
    if (bytesSent != buffer.size())
        throw std::exception("Error sending message to client");
}

RequestInfo Communicator::buildRequestInfo(const Buffer& buffer)
{
    RequestInfo info;

    // Extract the message code (first byte)
    info.messageCode = buffer[0];

    // Extract the message length (next 4 bytes)
    int length = (unsigned char)buffer[1] << 24 |
        (unsigned char)buffer[2] << 16 |
        (unsigned char)buffer[3] << 8 |
        (unsigned char)buffer[4];

    // Extract the message content (skip code and length)
    if (buffer.size() > 5) {
        info.buffer = Buffer(buffer.begin() + 5, buffer.end());
    }

    // Set receipt time
    info.timeRecieved = time(nullptr);

    return info;
}

void Communicator::handleNewClient(SOCKET clientSocket)
{
    try
    {
        // create a new LoginRequestHandler for this client
        IRequestHandler* handler = m_handlerFactory.createLoginRequestHandler();

        // add the client and its handler to the map
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_clients[clientSocket] = handler;
        }

        // handle client requests
        clientHandler(clientSocket);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error handling client: " << e.what() << std::endl;
        closesocket(clientSocket);
    }
}

void Communicator::clientHandler(SOCKET clientSocket)
{
    try
    {
        while (m_running)
        {
            // get the handler for this client
            IRequestHandler* handler = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto it = m_clients.find(clientSocket);
                if (it == m_clients.end())
                    break; // Client not found, exit

                handler = it->second;
            }

            if (!handler)
                break;

            try
            {
                // get message from client according to binary protocol
                Buffer buffer;
                try {
                    buffer = getMessageFromClient(clientSocket);
                }
                catch (const std::exception& e) {
                    std::string errorMsg = e.what();
                    if (errorMsg == "Error receiving message code") {
                        // Client disconnected - exit the loop
                        std::cout << "Client disconnected" << std::endl;
                        break;
                    }
                    // Re-throw other errors
                    throw;
                }

                // convert to RequestInfo
                RequestInfo requestInfo = buildRequestInfo(buffer);

                // check if the request is relevant for this handler
                if (handler->isRequestRelevant(requestInfo))
                {
                    // handle the request
                    RequestResult result = handler->handleRequest(requestInfo);

                    // send response to client
                    sendMessageToClient(clientSocket, result.response);

                    // update handler if needed
                    if (result.newHandler != handler)
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);
                        m_clients[clientSocket] = result.newHandler;
                        delete handler;
                    }
                }
                else
                {
                    // create error response
                    Response errorResponse;
                    errorResponse.status = 0;
                    errorResponse.message = "Request not relevant for current handler";
                    Buffer response = JsonResponsePacketSerializer::serializeErrorResponse(errorResponse);

                    // send error response
                    sendMessageToClient(clientSocket, response);
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error handling client message: " << e.what() << std::endl;

                // try to send error to client
                try
                {
                    Response errorResponse;
                    errorResponse.status = 0;
                    errorResponse.message = e.what();
                    Buffer response = JsonResponsePacketSerializer::serializeErrorResponse(errorResponse);
                    sendMessageToClient(clientSocket, response);
                }
                catch (...) {}
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error in client handler: " << e.what() << std::endl;
    }

    // clean up
    closesocket(clientSocket);

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_clients.find(clientSocket);
        if (it != m_clients.end())
        {
            delete it->second;
            m_clients.erase(it);
        }
    }
}

void Communicator::close()
{
    m_running = false;

    // Close the server socket
    if (m_serverSocket != INVALID_SOCKET)
    {
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
    }

    // Close all client sockets and delete handlers
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& pair : m_clients)
    {
        closesocket(pair.first);
        delete pair.second;
    }
    m_clients.clear();
}