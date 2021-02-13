#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodError : public AMethod
{
private:
    /* data */
    MethodError(/* args */);
public:
	MethodError(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodError();
    MethodStatus	createHeader();
    MethodStatus	createErrorHeader();
    MethodStatus	sendHeader(int socket);
    MethodStatus	readBody(int socket);
    MethodStatus	processRequest(std::string	const &location);
    MethodStatus	sendBody(int socket);
};
