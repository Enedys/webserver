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
	std::string			_remainder;
	std::string			_body;

public:
	MethodHead(int &status, RequestData const &data) :
		AMethod(status, data) { _type = HEAD;};
	~MethodHead();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendBody(int socket);

	virtual MethodStatus	sendResponse(int socket);
};
