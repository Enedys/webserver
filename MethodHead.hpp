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
		AMethod(config, status, headers) {};
	~MethodHead();
	MethodStatus	createHeader(std::string const &_path);
	MethodStatus	processBody(const std::string &requestBody);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};
