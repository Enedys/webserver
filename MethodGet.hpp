#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodGet: public AMethod
{
private:
    /* data */
	MethodGet(/* args */);
public:
	MethodGet(t_serv const &config, int &status, stringMap const &headers) :
		AMethod(config, status, headers) {};
	~MethodGet();
	MethodStatus	createHeader();
	MethodStatus	readRequestBody(int socket);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};
