#ifndef MethodOption_HPP
# define MethodOption_HPP

#pragma once
#include "Method.hpp"

class MethodOption : public AMethod
{
private:
	MethodOption(MethodOption const & src);
	MethodOption & operator=(MethodOption const & src);
	MethodOption();

public:
	MethodOption(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodOption();

	virtual MethodStatus	readRequestBody();
	virtual MethodStatus	manageRequest(std::string const &path);
	virtual MethodStatus	createHeader();
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	sendBody(int socket);
};

#endif
