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
	RequestData			&data;
	CGI					cgi;

	int					_fd;
	methodType			_type;
	bodyType			_bodyType;

	std::string			_output;
	cgiStatus			cgiStatus;

	size_t				_sentBytesTotal;
	size_t				_bytesToSend;
	static const size_t	_bs = 4096 * 448;
	std::string			_remainder;
	std::string			_body;
	int					_sendingInProgress;

	AMethod();
	AMethod(AMethod const & src);
	virtual AMethod &operator=(AMethod const & src);

	virtual size_t			defineRWlimits();
	virtual MethodStatus	readFromFileToBuf(size_t limit);
	virtual MethodStatus	sendBuf(int socket, std::string const & buf);

	virtual void			generateErrorPage(std::string &body);
	virtual int				generateIdxPage(std::string &body);

public:
	AMethod(int &status, RequestData &data);
	virtual ~AMethod();
	static const int			methodNums = 6;
	static const std::string	Methods[methodNums];
	static bool					isValidMethod(std::string const &method);

	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus) = 0;
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest() = 0;
	virtual MethodStatus	sendResponse(int socket);

	int						getStatusCode();
	CGI						&getCGI();
	bodyType				&getBodyType();
	int						&getFd();

};
