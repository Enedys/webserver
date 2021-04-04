#pragma once

#include <dirent.h>
#include "include_resources.hpp"
#include "RequestData.hpp"
#include "Parser.hpp"
#include "Header.hpp"
#include "CGI.hpp"

extern mapIntStr sc;

class AMethod
{
protected:
	int					&_statusCode;
	RequestData	const	&data;
	CGI					cgi;

	int					_fd;
	methodType			_type;
	bodyType			_bodyType;

	std::string			_output;
	cgiStatus			cgiStatus;

	size_t				_sentBytesTotal;
	size_t				_bytesToSend;
	static const size_t	_bs = 4096 * 512;
	std::string			_remainder;
	std::string			_body;

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
	virtual MethodStatus	sendBody(int socket);
	virtual MethodStatus	sendResponse(int socket) = 0;

	int						getStatusCode();
	CGI						&getCGI();
	bodyType				&getBodyType();
	int						&getFd();

	virtual void			generateErrorPage(std::string &body);
	virtual int				generateIdxPage(std::string &body);
};
