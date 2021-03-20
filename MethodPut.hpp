#pragma once
#include "Method.hpp"
#include "Header.hpp"

class MethodPut : public AMethod
{
private:
	MethodPut();
	MethodPut(MethodPut const & src);
	MethodPut & operator=(MethodPut const & src);
	Header				*_header;
	std::string			_remainder;
	std::string			_body;

public:
	MethodPut(int &status, RequestData const &data) :
		AMethod(status, data) { _type = PUT; };
	~MethodPut();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendBody(int socket);
	virtual MethodStatus	sendResponse(int socket);
};
