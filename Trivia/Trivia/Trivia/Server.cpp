#include "Server.hpp"
#include <iostream>
#include <string>

Server::Server() : m_handlerFactory(&m_database), m_communicator(m_handlerFactory), m_running(false)
{
    if (!m_database.open()) throw std::exception("Failed to open database");
}

Server::~Server()
{
	m_database.close();
	m_running = false;
}

void Server::run()
{
    m_running = true;


    // Start the communicator in a separate thread
    std::thread communicatorThread(&Communicator::startHandleRequests, &m_communicator);

    // Handle console input in the main thread
    handleConsoleInput();

    // Wait for the communicator thread to finish
    m_communicator.close();
    if (communicatorThread.joinable())
        communicatorThread.join();
}

void Server::handleConsoleInput()
{
    std::string command;

    while (m_running)
    {
        std::getline(std::cin, command);

        if (command == "exit")
        {
            std::cout << "Server shutting down..." << std::endl;
            m_running = false;
            break;
        }
        else
        {
            std::cout << "Server received command: " << command << std::endl;
            std::cout << "Type 'exit' to shutdown the server." << std::endl;
        }
    }
}

