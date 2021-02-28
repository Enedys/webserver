#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodOption : public AMethod
{
private:
    /* data */
	MethodOption(/* args */);
public:
	MethodOption(t_serv const &config, int &status, stringMap const &headers) : 
		AMethod(config, status, headers) {_type = OPTION;};
	~MethodOption();
	MethodStatus	createHeader();
	MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest();
	MethodStatus	sendBody(int socket);
};
