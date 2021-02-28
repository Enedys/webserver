#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodHead: public AMethod
{
private:
    /* data */
	MethodHead(/* args */);
public:
	MethodHead(int &status, RequestData const &data) :
		AMethod(status, data) {_type = HEAD;};
	~MethodHead();
	MethodStatus	createHeader();
	MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest();
	MethodStatus	sendBody(int socket);
};
