#pragma once
#include "includes.hpp"
#include "Debugger.hpp"
#include <algorithm>
#include <map>

typedef enum
{
	none,
	requestProc,
	responseReady,
}	RequestStatus;

class Request
{
	typedef std::map<std::string, std::string>::iterator	mapIter;
private:
	std::stringstream					request;
	std::map<std::string, std::string>	startLine;
	std::map<std::string, std::string>	headersMap;
	RequestStatus						_status;
	int									socket;
	std::string							_buffer;

	int				getRequest();
	int				processRequest();
	void			printRequest();
	std::string		getValue(std::string &s, std::string::iterator &sep);
	int				parseStartLine();
	int				validateStartLine();
	int				parseHeaders();
	int				validateHeaders();
	std::list<std::string>		_methods;
	Request();
public:
	RequestStatus		getRequestStatus() const;
	void				setRequestStatus(RequestStatus s);
	int					readRequest(Logger *_webLogger);
	virtual ~Request();
	Request(int fd) : _status(none), socket(fd)
	{
		_methods.push_back("GET");
		_methods.push_back("PUT");
		_methods.push_back("POST");
		_methods.push_back("HEAD");
	};

	// static std::list<std::string>		headers;
};

RequestStatus	Request::getRequestStatus() const
{
	return (_status);
}

void			Request::setRequestStatus(RequestStatus status)
{
	_status = status;
}

void			Request::printRequest()
{
	std::cout << "Method: " << startLine["method"] << " URI:" << startLine["uri"]\
	<< " Version: " << startLine["version"] << std::endl;
	mapIter	it = headersMap.begin();
	while (it != headersMap.end())
	{
		std::cout << "Header: " << it->first << ", Value: " << it->second << std::endl;
		it++;
	}
}

int			Request::processRequest()
{
	// if (std::find(headersMap.begin(), headersMap.end(), "host") == headersMap.end())
	// 	return (1);
	return (0);
}

int				Request::readRequest(Logger *_webLogger)
{
	char	buffer[512];
	int		read = 0;
	while ((read = recv(socket, buffer, 511, MSG_DONTWAIT)) > 0)
	{
		if (_webLogger)
			*_webLogger << verbose << "Read request from socket: "\
					<< socket << Logger::endl;
		buffer[read] = '\0';
		_buffer = buffer;
		request << buffer;
		size_t pos = _buffer.find(CRLF);
		if (_status == none && ((pos = _buffer.find(CRLF)) != std::string::npos))
		{
			*_webLogger << verbose << "Request gets" << Logger::endl;
			/* Here must be more complex error analysis */
			if (getRequest())
				return (1);
			printRequest();
			processRequest();
			_status = requestProc;
		}
		if (_status == requestProc) //else if
		{
			if (processRequest())
				return (1);
			_status = responseReady;
		}
	}
	if (read == 0)// && request.str().empty())
	{
		if (_webLogger)
			*_webLogger << reqread << "Сlient closed the connection" << Logger::endl;
		return (1);
	}
	// std::cout << "\033[33m MSG: \033[0m" << read<< "\n" << request.str() << std::endl;
	return (0);
}

int			Request::getRequest()
{
	if (parseStartLine()  == 1)
		return (1);
	if (validateStartLine() == 1)
		return (1);
	if (parseHeaders() == 1)
		return (1);
	if (validateHeaders() == 1)
		return (1);
	return (0);
}

std::string			Request::getValue(std::string &s, std::string::iterator &sep)
{
	std::string	value;
	if (*(++sep) == ' ')
		sep++;
	if (*(s.end() - 2) == ' ')
		value = s.substr(sep - s.begin(), s.end() - sep - 2);
	else
		value = s.substr(sep - s.begin(), s.end() - sep - 1);
	return (value);
}

int			Request::validateHeaders()
{
	return (0);
}

int			Request::parseHeaders()
{
	std::string				field;
	std::string				value;
	std::string				key;
	mapIter					inMap;
	std::string::iterator	pos;
	while(1)
	{
		std::getline(request, field, '\n');
		if (request.fail() || field[field.length() - 1] != '\r')
			return (1);
		if (field == "\r")
			return (0);
		if ((pos = std::find(field.begin(), field.end(), ':')) == field.end())
			return (1);
		key = field.substr(0, pos - field.begin());
		if (key.find(' ') != std::string::npos)
			return (1);
		for (int i = 0; i < key.length(); i++)
			key.at(i) = std::tolower(key.at(i));
		value = getValue(field, pos);
		inMap = headersMap.find(key);
		if (inMap != headersMap.end())
			headersMap[inMap->second] = inMap->second + "," + value;
		else
			headersMap[key] = value;	
	}
	return (0);
}

int			Request::validateStartLine()
{
	if (std::find(_methods.begin(), _methods.end(), startLine["method"]) == _methods.end())
		return (1);
	if (startLine["version"] != "HTTP/1.1")
		return (1);
	return (0);
}

int			Request::parseStartLine()
{
	std::string	arg;
	std::getline(request, arg, ' ');
	if (!request.eof() && !request.fail())
		startLine["method"] = arg;
	else
		return (1);
	std::getline(request, arg, ' ');
	if (!request.eof() && !request.fail())
		startLine["uri"] = arg;
	else
		return (1);
	std::getline(request, arg);
	if (!request.eof() && !request.fail()\
		&& arg[arg.length() - 1] == '\r')
		startLine["version"] = arg.substr(0, arg.length() - 1);
	else
		return (1);
	return (0);
}

Request::~Request()
{
}

// std::list<std::string> Request::methods({"GET", "HEAD", "POST", "PUT"});
// std::list<std::string> Request::headers({"Accept-Charsets", "Accept-Language", "Allow", "Authorization",\
// 						"Content-Language", "Content-Length", "Content-Location", "Content-Type",\
// 						"Date", "Host", "Last-Modified", "Location", "Referer", "Retry-After",\
// 						"Server", "Transfer-Encoding", "User-Agent", "WWW-Authenticate"});