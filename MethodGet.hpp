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
		AMethod(config, status, headers) {_type = GET; };
	~MethodGet();
	MethodStatus	createHeader(std::string const &_path);
	MethodStatus	processBody(const std::string &requestBody);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};
