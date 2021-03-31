#pragma once

/* Common used */

#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <list>
#include <vector>
#include <algorithm>

/* Client/Server includes */
#include "sys/types.h"
#include "sys/socket.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include "fcntl.h"
#include "unistd.h"

/* WebServer includes */
#include <sys/select.h>

/* Debugger includes */
#include <fstream>
#include <exception>
#include <ctime>

#define CRLF	"\r\n"

typedef	enum
{
	ok,
	inprogress,
	error,
	connectionClosed,
	socketError,
	logicError,
	allocationError,
}		MethodStatus;

typedef enum
{
	errorReadingURL = 1,	//may be replaced
	errorSocket = 1,		//may be replaced
	internalError = 1,

	errorSendingResponse = 1,	//may be replaced
	okWaitingMoreChunks = 1,//222 wtf
	okSendingInProgress = 999,
	okSuccess = 200,
	created = 201,
	accepted = 202,
	nocontent = 204,
	notFound = 404,
	errorOpeningURL = 403,//
	methodNotAllowed = 405,
	ImaTeapot = 418
}		statusCode;

typedef enum
{
	GET,
	HEAD,
	POST,
	PUT,
	OPTION,
}		methodType;

typedef enum
{
	bodyNotDefined,
	bodyIsEmpty,
	bodyIsAutoindex,		// text
	bodyIsTextErrorPage,	// text
	bodyIsFile,				// file: regularFile, indexFile, errorFile
	bodyIsCGI				// cgi
}		bodyType;

/* Typedefs */
typedef std::map<std::string, std::string>					stringMap;
typedef std::map<std::string, std::string>::iterator		mapIter;
typedef std::map<std::string, std::string>::const_iterator	constMapIter;

typedef std::map<int, std::string>							mapIntStr;
typedef std::map<int, std::string>::iterator				mapIntStrIter;
typedef std::map<int, std::string>::const_iterator			constMapIntStrIter;

int			string2Size(const std::string &str, int base=16);
std::string	size2Hex(size_t num, int base = 16);
std::string	size2Dec(size_t num, int base = 10);
void		stringToLower(std::string &s);
