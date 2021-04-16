#pragma once

#include <algorithm>
#include "Debugger.hpp"
#include "Method.hpp"
#include "include_resources.hpp"

#define MAX_URI_LENGTH		2048
#define	MAX_REQUEST_SIZE	8192
#define	BODY_BUFSIZE		(size_t)1048576 // 1 Mb

class Request
{
public:
	typedef enum
	{
		init,
		firstLine,
		headers,
		body
	}	requestStatus;

	Request(int fd, int &code);
	virtual ~Request();

	/* Public member functions */

	MethodStatus		readFromSocket();
	MethodStatus		getRequestHead();
	MethodStatus		getRequestBody(AMethod *method);
	MethodStatus		cleanRequest();
	size_t				getBufferResidual();
	requestStatus		getRequestState() const;
	const stringMap		&getHeadersMap() const;
	const stringMap		&getStartLine() const;
	std::string const	&getURI();
	std::string const	&getMethod();
	size_t				getContentLength();
	void				setBodyLimit(size_t limit);

private:
	Request();
	/* Fiels */
	static const size_t					_headBufsize = 1024;
	static const size_t					_bodyBufsize = 16386;
	std::map<std::string, std::string>	startLine;
	std::map<std::string, std::string>	headersMap;
	requestStatus						requestStage;
	std::string							_buffer;
	int									_socket;
	int									_bodySize;
	int									&_errorCode;
	size_t								_bodyLimit;
	size_t								chunkSize;
	bool 								end_body;
	int									trEnSize;
	int 								residBodysize;
	
	std::map<int, std::string>			_errors;

	/* Private member functions */
	MethodStatus	parseRequestHead(size_t posCRLF);
	MethodStatus	parseStartLine(size_t posCRLF);
	MethodStatus	parseHeaders();
	MethodStatus	validateStartLine();
	MethodStatus	validateHeaders();
	MethodStatus	getTrEncodedMsg(std::string &dest);

	MethodStatus	setErrorCode(int code);

	/* Debugging */
	void			printRequest();

	/* External functions */
	void			createErrorCodesMap();
};
