#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"

class AMethod
{
protected:
	stringMap const		&_headersMapRequest;
	t_serv const		&_config;
	int					&_statusCode;

	int					_fd;
	stringMap			_headersMap;
	methodType			_type;
	AMethod();

public:
	AMethod(t_serv const &config, int &status, stringMap const &headers);
	static const int			methodNums = 6;
	static const std::string	validMethods[6];
	static bool					isValidMethod(std::string const &method);

	virtual MethodStatus	createHeader(std::string const &_path) = 0;
	virtual MethodStatus	processBody(const std::string &requestBody) = 0;
	virtual MethodStatus	sendHeader(int socket) = 0;
	virtual MethodStatus	manageRequest(std::string const &location) = 0;
	virtual MethodStatus	sendBody(int socket) = 0;
	virtual ~AMethod();
};
