#pragma once

/* Common used */

#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <list>
#include <vector>

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

/* Typedefs */
typedef std::map<std::string, std::string>					stringMap;
typedef std::map<std::string, std::string>::iterator		mapIter;
typedef std::map<std::string, std::string>::const_iterator	constMapIter;
