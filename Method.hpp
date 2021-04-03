#pragma once

#include <dirent.h>
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
	Header				*_header;//do not need anymore
	CGI					cgi;

	int					_fd;
	stringMap			_headersMap;//будет не нужна, тк можно сразу в стр
	methodType			_type;
	bodyType			_bodyType;

	std::string			_output;
	cgiStatus			cgiStatus;
	AMethod();

public:
	AMethod(int &status, RequestData const &data);
	virtual ~AMethod();
	static const int			methodNums = 6;
	static const std::string	Methods[methodNums];
	static bool					isValidMethod(std::string const &method);

	virtual MethodStatus	createHeader() = 0;// create response
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus) = 0;
	virtual MethodStatus	sendHeader(int socket) = 0;
	virtual MethodStatus	manageRequest() = 0;
	virtual MethodStatus	sendBody(int socket) = 0;
	virtual MethodStatus	sendResponse(int socket) = 0;

	int						getStatusCode();
	CGI						&getCGI();
	bodyType				&getBodyType();
	int						&getFd();

	virtual std::string		&AMethod::generateErrorPage();//уже часть у Дани есть
	virtual std::string		&AMethod::generateIdxPage();
};