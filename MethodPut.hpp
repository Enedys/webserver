#pragma once
#include "Method.hpp"

class MethodPut : public AMethod
{
private:
    /* data */
	MethodPut(/* args */);
public:
	MethodPut(t_serv const &config, int &status, stringMap const &headers);
		// AMethod(config, status, headers) {};
	~MethodPut();
	MethodStatus	createHeader(std::string const &_path);
	MethodStatus	readRequestBody(int socket);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};
