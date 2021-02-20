#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"
#include "Request.hpp"
// #include "Header.hpp"// may be Header class -> Header template with functions

class AMethod
{
protected:
	const stringMap		&_headersMapRequest;	//from outside. ~ extract header info, e.g. about Transfer Encoding
	const t_serv		&_config;				//from outside
	int					&_statusCode;			//from client. Initially == 0 in client

	stringMap			_headersMap;			//local
	int					_fd;					//local. may add more fds for local use
	// std::string			_contentType;
	AMethod();

public:
	AMethod(t_serv const &config, int &status, stringMap const &headers);
	static const int			methodNums = 6;
	static const std::string	validMethods[6];
	static bool					isValidMethod(std::string const &method);
	virtual ~AMethod();

	virtual MethodStatus	readRequestBody(int socket) = 0;	// +path // for PUT POST, maybe
	virtual MethodStatus	manageRequest(std::string const &_path) = 0;	// path //check paths, fd open
	virtual MethodStatus	createHeader(std::string const &_path) = 0;		// +path (if error was at Request analyze stage) //according to _statusCode create OK or Error headers
	virtual MethodStatus	sendHeader(int socket) = 0;						// if error it was called from switch. if OK, just send. msg contains path
	virtual MethodStatus	sendBody(int socket) = 0;						// first sendHeader, then manip with body
	// virtual MethodStatus	sendResponse(int socket) = 0;
	// void					headersToString(stringMap const &headersMap, std::string *output);
};