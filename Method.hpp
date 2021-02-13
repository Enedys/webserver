#pragma once
#include "includes.hpp"

class AMethod
{
protected:
	stringMap			_headersMap;
	Request const		*_request;
	int					_status;
	int					_fd;
	const serv_config	*_config;
	AMethod();
public:
	AMethod(serv_config const *config, Request const *request) : _config(config), _request(request) {};
	virtual MethodStatus	createHeader();
	virtual MethodStatus	createErrorHeader();
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	readBody(int socket);
	virtual MethodStatus	processRequest(std::string	const &location);
	virtual MethodStatus	sendBody(int socket);
	virtual ~AMethod() = 0;
};

AMethod::AMethod() {};

AMethod::~AMethod() {};
