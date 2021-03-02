#pragma once
#include "Method.hpp"
#include "Header.hpp"

#include <dirent.h>
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
	MethodGet(int &status, RequestData const &data) : AMethod(status, data) {_type = GET; };
	~MethodGet();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendBody(int socket);

	virtual MethodStatus	sendResponse(int socket);

	int						generateIdxPage(std::string const &_path);
};
