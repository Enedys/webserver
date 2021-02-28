#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodGet: public AMethod
{
private:
    /* data */
	MethodGet(/* args */);
public:
	MethodGet(int &status, RequestData const &data) :
		AMethod(status, data) {_type = GET; };
	~MethodGet();
	MethodStatus	createHeader();
	MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest();
	MethodStatus	sendBody(int socket);
};
