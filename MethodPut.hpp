#ifndef MethodPut_HPP
# define MethodPut_HPP

#pragma once
#include "Method.hpp"

class MethodPut : public AMethod
{
private:
	MethodPut(MethodPut const & src);
	MethodPut & operator=(MethodPut const & src);
	MethodPut();

public:
	MethodPut(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodPut();

	virtual MethodStatus	readRequestBody();
	virtual MethodStatus	manageRequest(std::string const &path);
	virtual MethodStatus	createHeader();
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	sendBody(int socket);
};

#endif