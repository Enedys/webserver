#pragma once
#include "include_resources.hpp"
#include "RequestData.hpp"
#include "Parser.hpp"
#include "Header.hpp"
#include "CGI.hpp"

class AMethod
{
protected:
	int					&_statusCode;
	RequestData	const	&data;
	Header				*_header;
	CGI					cgi;

	int					_fd;
	stringMap			_headersMap;
	methodType			_type;
	bodyType			_bodyType;
	AMethod();

public:
	AMethod(int &status, RequestData const &data);
	virtual ~AMethod();
	static const int			methodNums = 6;
	static const std::string	Methods[methodNums];
	static bool					isValidMethod(std::string const &method);

	virtual MethodStatus	createHeader() = 0;
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus) = 0;
	virtual MethodStatus	sendHeader(int socket) = 0;
	virtual MethodStatus	manageRequest() = 0;
	virtual MethodStatus	sendBody(int socket) = 0;
	virtual MethodStatus	sendResponse(int socket) = 0;

	int						getStatusCode();
	CGI						&getCGI();
	bodyType				&getBodyType();
	int						&getFd();
};