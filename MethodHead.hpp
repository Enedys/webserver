#ifndef MethodHead_HPP
# define MethodHead_HPP

#pragma once
#include "Method.hpp"

class MethodHead : public AMethod
{
private:
    /* data */
	MethodHead(/* args */);
	MethodHead(MethodHead const & src);
	MethodHead & operator=(MethodHead const & src);
public:
	MethodHead(t_serv const &config, int &status, stringMap const &headers) :
	AMethod(config, status, headers) {};
	~MethodHead();
	MethodStatus	createHeader();
	MethodStatus	readRequestBody(int socket);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};

#endif
