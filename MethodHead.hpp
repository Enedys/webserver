#ifndef MethodHead_HPP
# define MethodHead_HPP

#pragma once
#include "Method.hpp"

class MethodHead : public AMethod
{
private:
	MethodHead(MethodHead const & src);
	MethodHead & operator=(MethodHead const & src);
	MethodHead();

public:
	MethodHead(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodHead();

	virtual MethodStatus	readRequestBody();
	virtual MethodStatus	manageRequest(std::string const &path);
	virtual MethodStatus	createHeader();
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	sendBody(int socket);
};

#endif
