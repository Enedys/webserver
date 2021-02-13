#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodPost: public AMethod
{
private:
    /* data */
    MethodPost(/* args */);
public:
	MethodPost(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodPost();
    MethodStatus	createHeader();
    MethodStatus	createErrorHeader();
    MethodStatus	sendHeader(int socket);
    MethodStatus	readBody(int socket);
    MethodStatus	processRequest(std::string	const &location);
    MethodStatus	sendBody(int socket);
};
