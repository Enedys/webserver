#pragma once
#include "Method.hpp"

class MethodHead : public AMethod
{
private:
	MethodHead();
	MethodHead(MethodHead const & src);
	MethodHead & operator=(MethodHead const & src);

public:
	MethodHead(int &status, RequestData const &data) :
		AMethod(status, data) { _type = HEAD;};
	~MethodHead();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendResponse(int socket);
};
