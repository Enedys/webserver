#pragma once
#include <dirent.h>
#include <iostream>

#include "Method.hpp"
#include "Header.hpp"

//max body size

class MethodGet : public AMethod
{
private:
	MethodGet();
	MethodGet(MethodGet const & src);
	MethodGet & operator=(MethodGet const & src);
	Header				*_header;
	size_t				_sentBytesTotal;
	size_t				_bytesToSend;
	static const size_t	_bs = 4096;
	std::string			_remainder;
	std::string			_body;

public:
	MethodGet(int &status, RequestData const &data) : AMethod(status, data) {
		_type = GET;
		_sentBytesTotal = 0;
		// _bytesToSend = 0;
	};
	~MethodGet();
	virtual MethodStatus	createHeader();//2
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);//3
	virtual MethodStatus	manageRequest();//1
	virtual MethodStatus	sendBody(int socket);//4

	virtual MethodStatus	sendResponse(int socket);

	void					generateIdxPage();
	int						generateErrorPage();
};
