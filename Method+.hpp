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
	int					_status;//is this 401 or just ok processing
	int					_fd;
	const t_serv		*_config;
	Header				*_header;
	AMethod();
public:
	AMethod(t_serv const *config, Request const *request) : _config(config), _request(request) {};
	virtual MethodStatus	createHeader() = 0;
	virtual MethodStatus	createErrorHeader() = 0;
	virtual MethodStatus	sendHeader(int socket) = 0;
	virtual MethodStatus	readBody(int socket) = 0;
	virtual MethodStatus	processRequest(std::string	const &location) = 0;
	virtual MethodStatus	sendBody(int socket) = 0;
	virtual ~AMethod();
};


class AMethod
{
protected:
	stringMap			_headersMap;
	Request const		*_request;
	int					_status;
	int					_fd;
	const t_serv		*_config;
	Header				*_header;
	AMethod();
public:
	AMethod(t_serv const *config, Request const *request) : _config(config), _request(request) {};
	virtual MethodStatus	createHeader() = 0;
	virtual MethodStatus	createErrorHeader() = 0;
	virtual MethodStatus	sendHeader(int socket) = 0;
	virtual MethodStatus	readBody(int socket) = 0;
	virtual MethodStatus	processRequest(std::string	const &location) = 0;
	virtual MethodStatus	sendBody(int socket) = 0;
	virtual ~AMethod();
};

class MethodGet : public AMethod
	MethodGet(t_serv const *config, Request	const *_request);
	~MethodGet();

	MethodStatus	createHeader();
	MethodStatus	createErrorHeader();
	MethodStatus	sendHeader(int socket);
	MethodStatus	processRequest(std::string const &path);
	MethodStatus	sendBody(int socket);
	MethodStatus	readBody(int socket);

