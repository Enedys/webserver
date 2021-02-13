#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodOption : public AMethod
{
private:
    /* data */
    MethodOption(/* args */);
public:
	MethodOption(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodOption();
    MethodStatus	createHeader();
    MethodStatus	createErrorHeader();
    MethodStatus	sendHeader(int socket);
    MethodStatus	readBody(int socket);
    MethodStatus	processRequest(std::string	const &location);
    MethodStatus	sendBody(int socket);
};
