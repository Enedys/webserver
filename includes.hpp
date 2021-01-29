#pragma once

/* Common used */
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

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