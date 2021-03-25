#pragma once
#include "Method.hpp"

class MethodOption : public AMethod
{
private:
	MethodOption();
	MethodOption(MethodOption const & src);
	MethodOption & operator=(MethodOption const & src);
	std::string			_remainder;
	std::string			_body;//if autoindex, if Options will be not allowed by config

public:
	MethodOption(int &status, RequestData const &data) :
		AMethod(status, data) {_type = OPTION; };
	~MethodOption();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendBody(int socket);
	virtual MethodStatus	sendResponse(int socket);
};
