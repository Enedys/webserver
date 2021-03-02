#pragma once
#include "Method.hpp"

class MethodOption : public AMethod
{
private:
    /* data */
	MethodOption(/* args */);
public:
	MethodOption(int &status, RequestData const &data) : 
		AMethod(status, data) {_type = OPTION;};
	~MethodOption();
	MethodStatus	createHeader();
	MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest();
	MethodStatus	sendBody(int socket);
	virtual MethodStatus	sendResponse(int socket);
};
