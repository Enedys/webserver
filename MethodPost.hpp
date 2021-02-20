#pragma once
#include "Method.hpp"

class MethodPost : public AMethod
{
private:
    /* data */
	MethodPost(/* args */);
public:
	MethodPost(t_serv const &config, int &status, stringMap const &headers);
		// AMethod(config, status, headers) {};
	~MethodPost();
	MethodStatus	createHeader(std::string const &_path);
	MethodStatus	readRequestBody(int socket);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};
