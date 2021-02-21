#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"

class AMethod
{
protected:
	const stringMap		&_headersMapRequest;	//from outside. ~ extract header info, e.g. about Transfer Encoding
	const t_serv		&_config;
	int					&_statusCode;

	stringMap			_headersMap;
	int					_fd;
	AMethod();

public:
	AMethod(t_serv const &config, int &status, stringMap const &headers);
	static const int			methodNums = 6;
	static const std::string	validMethods[6];
	static bool					isValidMethod(std::string const &method);
	virtual ~AMethod();

	virtual MethodStatus	readRequestBody(int socket) = 0;				// +path // for PUT POST, maybe
	virtual MethodStatus	manageRequest(std::string const &_path) = 0;	// path //check paths, fd open
	virtual MethodStatus	createHeader(std::string const &_path) = 0;		// +path (if error was at Request analyze stage) //according to _statusCode create OK or Error headers
	virtual MethodStatus	sendHeader(int socket) = 0;						// if error it was called from switch. if OK, just send. msg contains path
	virtual MethodStatus	sendBody(int socket) = 0;						// first sendHeader, then manip with body

	int getStatusCode() { return _statusCode; };
	// virtual MethodStatus	sendResponse(int socket) = 0;//
};