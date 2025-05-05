#pragma once
#include <ctime>
#include <vector>

typedef unsigned char Byte;
typedef std::vector<Byte> Buffer;

struct RequestInfo
{
	int messageCode = 0;
	time_t timeRecieved = 0;
	Buffer buffer;
};
