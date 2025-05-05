#include "WSAInitializer.h"
#include "Server.h"
#include <iostream>
#include <exception>

#pragma comment(lib, "ws2_32")

int main()
{
    try
    {
        WSAInitializer wsaInit;
        Server server;
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}