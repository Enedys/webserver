<<<<<<< HEAD
=======
#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"

class AMethod
{
protected:
	stringMap const		&_headersMap;
	t_serv const		&_config;
	int					&_status;
	AMethod();

public:
	AMethod(t_serv const &config, int &status, stringMap const &headers);
	static const int			methodNums = 6;
	static const std::string	validMethods[6];
	static bool					isValidMethod(std::string const &method);

	virtual MethodStatus	createHeader() = 0;
	virtual MethodStatus	readRequestBody(int socket) = 0;
	virtual MethodStatus	sendHeader(int socket) = 0;
	virtual MethodStatus	manageRequest(std::string const &location) = 0;
	virtual MethodStatus	sendBody(int socket) = 0;
	virtual ~AMethod();
};
>>>>>>> ca122da6260ff34786d0946ed1381ace59bcb16c
