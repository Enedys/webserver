#pragma once
// class Logger;

#include <algorithm>//added because it did not compile
#include "Debugger.hpp"
//#include "include_resources.hpp"

#define MAX_URI_LENGTH		4096
#define	MAX_REQUEST_SIZE	8192


class Request
{

public:
	typedef enum
	{
		none,
		firstLine,
		headersProc,
		bodyProc,
		responseReady,
	}	RequestStatus;

	const std::list<std::string>	getValidMethod() const {return (_methods); };
	RequestStatus		getRequestStatus() const { return (_status); };
	const stringMap		getStartLine() const {return (startLine); };
	const stringMap		getHeadersMap() const { return (headersMap); };
	const int			getErrorCode() const { return (_errorCode); };
	const int			getSocket() const { return (socket); };
	void				setRequestStatus(RequestStatus s);
	int					readRequest(Logger *_webLogger);
	virtual ~Request();
	Request(int fd);
private:
	std::map<std::string, std::string>	startLine;
	std::map<std::string, std::string>	headersMap;
	RequestStatus						_status;
	int									socket;
protected:
	int									_errorCode;
	int									_bodySize;
	std::string							_buffer;

	int				processRequest();

	void			printRequest();
	int				parseStartLine();
	int				validateStartLine();
	int				parseHeaders();
	int				validateHeaders();
	std::list<std::string>				_methods;
	std::map<int, std::string>			_errors;
	void			setErrorCodes();
	Request();
};


// std::list<std::string> Request::methods({"GET", "HEAD", "POST", "PUT"});
// std::list<std::string> Request::headers({"Accept-Charsets", "Accept-Language", "Allow", "Authorization",\
// 						"Content-Language", "Content-Length", "Content-Location", "Content-Type",\
// 						"Date", "Host", "Last-Modified", "Location", "Referer", "Retry-After",\
// 						"Server", "Transfer-Encoding", "User-Agent", "WWW-Authenticate"});