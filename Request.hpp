#pragma once
#include "includes.hpp"
#include "Debugger.hpp"
#include <algorithm>
#include <map>

class Request
{
private:
	std::stringstream					request;
	std::map<std::string, std::string>	startLine;
	bool								_isRecieved;
	int									socket;

	static std::list<std::string>		methods;
	static std::list<std::string>		headers;
	int				parseStartLine();
	int				validateStartLine();
	int				parseHeaders();
	int				validateHeaders();
	Request();
public:
	bool				requestRecieved() const;
	void				setWaitRequestStatus(bool s);
	int					readRequest(Logger *_webLogger);
	virtual ~Request();
	Request(int &fd) : _isRecieved(0), socket(fd) {};
};

bool			Request::requestRecieved() const
{
	return (_isRecieved);
}

void			Request::setWaitRequestStatus(bool s)
{
	_isRecieved = s;
}

int			Request::validateStartLine()
{
	if (std::find(methods.begin(), methods.end(), startLine["method"]) == methods.end())
		return (1);
	if (startLine["version"] != "HTTP/1.1")
		return (1);
	return (0);
}

int				Request::readRequest(Logger *_webLogger)
{
	char	buffer[1024];
	int		read = 0;
	while ((read = recv(socket, buffer, 1023, MSG_DONTWAIT)) > 0)
	{
		if (_webLogger)
			*_webLogger << verbose << "Read request from socket: "\
					<< socket << Logger::endl;
		buffer[read] = '\0';
		request << buffer;
	}
	if (request.str().empty() && read == 0)
	{
		if (_webLogger)
			*_webLogger << reqread << "Сlient closed the connection" << Logger::endl;
		return (1);
	}
	_isRecieved = 1;
	std::cout << "\033[33m MSG: \033[0m \n" << request.str() << std::endl;
	/* This action must be in response processing */
	request.clear();
	request.str("");
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
		startLine["version"] = arg;
	else
		return (1);
	return (0);
}

Request::~Request()
{
}

std::list<std::string> Request::methods = \
std::list<std::string>({"GET", "HEAD", "POST", "PUT", "DELETE"});

std::list<std::string> Request::headers = \
std::list<std::string>({"Accept-Charsets", "Accept-Language", "Allow", "Authorization",\
						"Content-Language", "Content-Length", "Content-Location", "Content-Type",\
						"Date", "Host", "Last-Modified", "Location", "Referer", "Retry-After",\
						"Server", "Transfer-Encoding", "User-Agent", "WWW-Authenticate"});