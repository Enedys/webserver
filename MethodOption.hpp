#ifndef MethodOption_HPP
# define MethodOption_HPP

#pragma once
#include "Method.hpp"

class MethodOption : public AMethod
{
private:
    /* data */
	MethodOption(/* args */);
public:
	MethodOption(t_serv const &config, int &status, stringMap const &headers) :
		AMethod(config, status, headers) {};
	~MethodOption();
	MethodStatus	createHeader();
	MethodStatus	readRequestBody(int socket);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};

#endif
