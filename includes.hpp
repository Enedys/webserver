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
#include <list>

/* Debugger includes */
#include <fstream>
#include <exception>
#include <ctime>

/* Typedefs */
typedef std::map<std::string, std::string>					stringMap;
typedef std::map<std::string, std::string>::iterator		mapIter;
typedef std::map<std::string, std::string>::const_iterator	constMapIter;

/* Andrew */
#include "Parser.hpp"
typedef Parser::t_serv										serv_config;

typedef	enum
{
	ok,
	inprogress,
	error
}		MethodStatus;

/* Methods */
#include "Method.hpp"
#include "MethodGet.hpp"
#include "MethodHead.hpp"
#include "MethodPut.hpp"
#include "MethodPost.hpp"
#include "MethodOption.hpp"
#include "MethodError.hpp"

#define CRLF	"\r\n"
