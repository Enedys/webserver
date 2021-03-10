#pragma once
#include <dirent.h>
#include "Method.hpp"
#include "Header.hpp"

class MethodDelete : public AMethod
{
private:
	MethodDelete();
	MethodDelete(MethodDelete const & src);
	MethodDelete & operator=(MethodDelete const & src);
	Header				*_header;
	std::string			_remainder;
	std::string			_body;//if autoindex, if Options will be not allowed by config

public:
	MethodDelete(int &status, RequestData const &data) :
		AMethod(status, data) {_type = OPTION; };
	~MethodDelete();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendBody(int socket);
	virtual MethodStatus	sendResponse(int socket);

	int						deleteDirectory(std::string const &path);
};
