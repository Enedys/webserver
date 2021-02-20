#pragma once
#include "Debugger.hpp"
#include "Method.hpp"
#include "include_resources.hpp"

#define MAX_URI_LENGTH		4096
#define	MAX_REQUEST_SIZE	8192


class Request
{
	typedef enum
	{
		firstLine,
		headers,
		body,
	}	requestStatus;
public:
	Request(int fd, int &code);
	virtual ~Request();

	/* Public member functions */
	size_t				getBufferResidual();
	MethodStatus		cleanRequest();
	MethodStatus		readRequestHead(Logger *_webLogger);
	MethodStatus		readRequestBody(AMethod *method, Logger *_webLogger);
	const stringMap		getHeadersMap() const;
	const stringMap		getStartLine() const;
	std::string const	&getURI() const;

private:
	Request();
	/* Fiels */
	static const int					_buffer_size = 2048;
	requestStatus						readingStage;
	std::map<std::string, std::string>	startLine;
	std::map<std::string, std::string>	headersMap;
	std::string							_buffer;
	int									_socket;
	int									&_errorCode;
	size_t								_bodySize;
	std::map<int, std::string>			_errors;

	/* Private member functions */
	MethodStatus	setErrorCode(int code);
	MethodStatus	parseStartLine(size_t posCRLF);
	MethodStatus	parseHeaders();
	MethodStatus	validateStartLine();
	MethodStatus	validateHeaders();

	/* Debugging */
	void			printRequest();

	/* External functions */
	void			createErrorCodesMap();
};
