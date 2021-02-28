#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodPost: public AMethod
{
private:
    /* data */
    MethodPost(/* args */);
public:
	MethodPost(t_serv const &config, int &status, stringMap const &headers) :
		AMethod(config, status, headers) {_type = POST;};
	virtual ~MethodPost();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendBody(int socket);
};
