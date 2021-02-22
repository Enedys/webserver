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
	MethodStatus	createHeader(std::string const &_path);
	MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};
