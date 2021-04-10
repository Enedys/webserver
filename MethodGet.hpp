#pragma once

#include "Method.hpp"

class MethodGet : public AMethod
{
private:
	MethodGet();
	MethodGet(MethodGet const & src);
	MethodGet & operator=(MethodGet const & src);

public:
	MethodGet(int &status, RequestData &data) :
		AMethod(status, data) { _type = GET; };
	~MethodGet();
	// virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
};
