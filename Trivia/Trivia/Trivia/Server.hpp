#pragma once
#include "Communicator.hpp"
#include "RequestHandlerFactory.hpp"
#include "SqliteDatabase.hpp"
#include <thread>

class Server
{
public:
    Server();
    ~Server();

    void run();

private:
    void handleConsoleInput();

    SqliteDatabase m_database;
    RequestHandlerFactory m_handlerFactory;
    Communicator m_communicator;
    bool m_running;
};

