#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"
#include "Request.hpp"

// class MethodError;
// class MethodGet;
// class MethodHead;
// class MethodOption;
// class MethodPost;
// class MethodPut;

class AMethod
{
protected:
	stringMap			_headersMap;
	Request const		*_request;
	int					_status;
	int					_fd;
	const t_serv		*_config;
	AMethod();
public:
	AMethod(t_serv const *config, Request const *request) : _config(config), _request(request) {};
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
