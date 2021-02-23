#pragma once
#include "Method.hpp"
#include "Header.hpp"

class MethodHead : public AMethod
{
private:
	MethodHead();
	MethodHead(MethodHead const & src);
	MethodHead & operator=(MethodHead const & src);
	Header				*_header;
	static const size_t	_bs = 4096;
	std::string			_remainder;
public:
	MethodHead(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodHead();
	virtual MethodStatus	createHeader(std::string const &_path);
	virtual MethodStatus	readRequestBody(int socket);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest(std::string const &location);
	virtual MethodStatus	sendBody(int socket);
	virtual MethodStatus	sendResponse(int socket);
};
