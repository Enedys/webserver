#pragma once
#include "Debugger.hpp"
#include "Method.hpp"
#include "include_resources.hpp"

#define MAX_URI_LENGTH		2048
#define	MAX_REQUEST_SIZE	8192
#define	BODY_BUFSIZE		(size_t)1048576 // 1 Mb


class Request
{
	typedef enum
	{
		init,
		firstLine,
		headers,
		body,
	}	requestStatus;
public:
	Request(int fd, int &code);
	virtual ~Request();

	/* Public member functions */

	MethodStatus		readFromSocket();
	MethodStatus		getRequestHead();
	MethodStatus		getRequestBody(AMethod *method);
	MethodStatus		cleanRequest();
	size_t				getBufferResidual();
	MethodStatus		getLastReadStatus() const;
	const stringMap		getHeadersMap() const;
	const stringMap		getStartLine() const;
	std::string const	&getURI();

private:
	Request();
	/* Fiels */
	static const size_t					_headBufsize = 1024;
	static const size_t					_bodyBufsize = 16384;
	std::map<std::string, std::string>	startLine;
	std::map<std::string, std::string>	headersMap;
	requestStatus						requestStage;
	std::string							_buffer;
	int									_socket;
	size_t								_bodySize;
	int									&_errorCode;
	MethodStatus						_lastReadStatus;
	
	std::map<int, std::string>			_errors;

	/* Private member functions */
	MethodStatus	parseRequestHead(size_t posCRLF);
	MethodStatus	parseStartLine(size_t posCRLF);
	MethodStatus	parseHeaders();
	MethodStatus	validateStartLine();
	MethodStatus	validateHeaders();
	MethodStatus	readEncodedBody(std::string &dest);
	MethodStatus	readOrdinaryBody(std::string &dest);
	
	MethodStatus	setErrorCode(int code);
	MethodStatus	setLastReadStatus(MethodStatus status);

	/* Debugging */
	void			printRequest();

	/* External functions */
	void			createErrorCodesMap();
};
