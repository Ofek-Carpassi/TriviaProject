#pragma once

#include "RequestInfo.hpp"
#include "ResponseStructs.hpp"

class IRequestHandler;

typedef struct RequestResult
{
	Buffer response;
	IRequestHandler* newHandler;
} RequestResult;

class IRequestHandler
{
public:
	virtual ~IRequestHandler() = default;
	virtual bool isRequestRelevant(const RequestInfo& request) = 0;
	virtual RequestResult handleRequest(const RequestInfo& request) = 0;
};

