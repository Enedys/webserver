#pragma once
#include "includes.hpp"
#include "Method.hpp"
#include "Request.hpp"

class Response
{
private:
	Request			*request;
	IMethod			*method;
	int				setRequest();
	RequestStatus	prepareResponse();
	int				sendRequest();
	std::string		getLocation();
	
	/* data */
public:
	Response(/* args */);
	~Response();
};

ReuqestStatus		Response::prepareRequest()
{
	if (requset)
	{
		switch
	}
}

Response::Response() : request(NULL), method(NULL) {};

Response::~Response() {};
