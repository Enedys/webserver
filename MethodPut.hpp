#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodPut: public AMethod
{
private:
    /* data */
	MethodPut(/* args */);
public:
	MethodPut(t_serv const &config, int &status, stringMap const &headers) : 
		AMethod(config, status, headers) {_type = PUT; };
	~MethodPut();
	MethodStatus	createHeader();
	MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest();
	MethodStatus	sendBody(int socket);
};
