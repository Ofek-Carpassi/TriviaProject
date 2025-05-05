#pragma once
#include "Communicator.h"
#include "RequestHandlerFactory.h"
#include "SqliteDatabase.h"
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
