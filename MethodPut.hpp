#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodPut: public AMethod
{
private:
    /* data */
    MethodPut(/* args */);
public:
	MethodPut(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodPut();
    MethodStatus	createHeader();
    MethodStatus	createErrorHeader();
    MethodStatus	sendHeader(int socket);
    MethodStatus	readBody(int socket);
    MethodStatus	processRequest(std::string	const &location);
    MethodStatus	sendBody(int socket);
};
