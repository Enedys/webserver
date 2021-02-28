#pragma once
#include "include_resources.hpp"
#include "RequestData.hpp"
#include "Parser.hpp"

class AMethod
{
protected:
	// stringMap const		&_headersMapRequest;
	// t_serv const		&_config;
	int					&_statusCode;
	RequestData	const	&data;

	int					_fd;
	stringMap			_headersMap;
	methodType			_type;
	AMethod();

public:
	AMethod(int &status, RequestData const &data);
	static const std::string	validMethods[6];
	static const int			methodNums = 6;
	static bool					isValidMethod(std::string const &method);

	virtual MethodStatus	createHeader() = 0;
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus) = 0;
	virtual MethodStatus	sendHeader(int socket) = 0;
	virtual MethodStatus	manageRequest() = 0;
	virtual MethodStatus	sendBody(int socket) = 0;
	virtual ~AMethod();
};
