#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodHead: public AMethod
{
private:
    /* data */
    MethodHead(/* args */);
public:
	MethodHead(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodHead();
    MethodStatus	createHeader();
    MethodStatus	createErrorHeader();
    MethodStatus	sendHeader(int socket);
    MethodStatus	readBody(int socket);
    MethodStatus	processRequest(std::string	const &location);
    MethodStatus	sendBody(int socket);
};
