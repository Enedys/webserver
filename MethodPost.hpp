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
	MethodPost(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodPost();

	virtual MethodStatus	readRequestBody();
	virtual MethodStatus	manageRequest(std::string const &path);
	virtual MethodStatus	createHeader();
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	sendBody(int socket);
};

#endif
