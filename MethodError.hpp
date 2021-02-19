#ifndef MethodError_HPP
# define MethodError_HPP

#pragma once
#include "Method.hpp"

class MethodError : public AMethod
{
private:
	MethodError(MethodError const & src);
	MethodError & operator=(MethodError const & src);
	MethodError();

public:
	MethodError(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodError();

	virtual MethodStatus	readRequestBody();
	virtual MethodStatus	manageRequest(std::string const &path);
	virtual MethodStatus	createHeader();
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	sendBody(int socket);
};

#endif
