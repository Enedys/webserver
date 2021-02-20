#ifndef MethodPost_HPP
# define MethodPost_HPP

#pragma once
#include "Method.hpp"

class MethodPost : public AMethod
{
private:
	MethodPost(MethodPost const & src);
	MethodPost & operator=(MethodPost const & src);
	MethodPost();

public:
	MethodPost(t_serv const &config, int &status, stringMap const &headers) :
		AMethod(config, status, headers) {};
	~MethodPost();
	MethodStatus	createHeader();
	MethodStatus	readRequestBody(int socket);
	MethodStatus	sendHeader(int socket);
	MethodStatus	manageRequest(std::string const &location);
	MethodStatus	sendBody(int socket);
};

#endif
