#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodPut: public AMethod
{
private:
    /* data */
	MethodPut(/* args */);
public:
	MethodPut(int &status, RequestData const &data) : 
		AMethod(status, data) {_type = PUT; };
	~MethodPut();
	MethodStatus	createHeader();
	MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest();
	MethodStatus	sendBody(int socket);
};
