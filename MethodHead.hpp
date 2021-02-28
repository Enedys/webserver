#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodHead: public AMethod
{
private:
    /* data */
	MethodHead(/* args */);
public:
	MethodHead(t_serv const &config, int &status, stringMap const &headers) :
		AMethod(config, status, headers) {_type = HEAD;};
	~MethodHead();
	MethodStatus	createHeader();
	MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest();
	MethodStatus	sendBody(int socket);
};
