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
	socketError,
	logicError,
}		MethodStatus;

typedef enum
{
	errorReadingURL = 1,	//may be replaced
	errorSocket = 1,		//may be replaced

	errorSendHeader = 1,	//may be replaced
	okWaitingMoreChunks = 1,//222 wtf
	okSuccess = 200,
	created = 201,
	accepted = 202,
	nocontent = 204,
	notFound = 404,
	errorOpeningURL = 403//
}		statusCode;

/* Typedefs */
typedef std::map<std::string, std::string>					stringMap;
typedef std::map<std::string, std::string>::iterator		mapIter;
typedef std::map<std::string, std::string>::const_iterator	constMapIter;
