#ifndef MethodHead_HPP
# define MethodHead_HPP

#pragma once
#include "Method.hpp"
#include "Header.hpp"

# define BUFSIZE 4096

class MethodHead : public AMethod
{
private:
    /* data */
	MethodHead(/* args */);
	MethodHead(MethodHead const & src);
	MethodHead & operator=(MethodHead const & src);
	Header		*_header;
public:
	MethodHead(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodHead();
	MethodStatus	createHeader(std::string const &_path);
	MethodStatus	readRequestBody(int socket);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};

#endif
