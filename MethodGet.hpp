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
	MethodGet(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodGet();

	virtual MethodStatus	readRequestBody(int socket);	//+path for cgi
	virtual MethodStatus	manageRequest(std::string const &path);
	virtual MethodStatus	createHeader(std::string const &_path);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	sendBody(int socket);
	virtual MethodStatus	sendResponse(int socket);

	int						generateIdxPage(std::string const &_path);
};
