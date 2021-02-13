#include "Request.hpp"

Request::Request(int fd) : _status(none), socket(fd), _bodySize(0), _errorCode(0)
{
	_methods.push_back("GET");
	_methods.push_back("PUT");
	_methods.push_back("POST");
	_methods.push_back("HEAD");
	_methods.push_back("DELETE");
	_methods.push_back("OPTIONS");
	setErrorCodes();
};

void			Request::setErrorCodes()
{
	_errors[400] = "Bad Request";
	_errors[404] = "Not Found";
	_errors[414] = "URI Too Long";
	_errors[501] = "Not Implemented";
	_errors[505] = "HTTP Version Not Supported";
}

void			Request::setRequestStatus(RequestStatus status) { _status = status; }

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
			*_webLogger << Message::verbose << "Read request from socket: "\
					<< socket << Logger::endl;
		buffer[read] = '\0';
		_buffer += buffer;
		size_t pos = _buffer.find(CRLF);
		std::cout << buffer << std::endl;
		if (_status == none && pos != std::string::npos)
			if (parseStartLine())
				return (1);
		if (parseHeaders())
			return (1);
		if (_status < bodyProc && _buffer.length() > MAX_REQUEST_SIZE)
			return ((_errorCode = 400));
		if (_status == headersProc)
		{
			if (validateHeaders())
				return (1);
			else
				_status = bodyProc;
		}
		printRequest();
		if (_status == bodyProc)
		{
			_status = responseReady;
		}
	}
	if (read == 0)// && request.str().empty())
	{
		if (_webLogger)
			*_webLogger << Message::reqread << "Сlient closed the connection" << Logger::endl;
		return (1);
	}
	return (0);
}

int			Request::validateHeaders()
{
	if (headersMap["host"] == "")
		return ((_errorCode = 400));
	mapIter	trEn = headersMap.find("transfer-encoding");
	mapIter	contLen = headersMap.find("content-length");
	if (trEn != headersMap.end() && contLen != headersMap.end())
		return ((_errorCode = 400));
	if (trEn == headersMap.end() && contLen == headersMap.end())
		_bodySize = 0;
	if (contLen != headersMap.end())
	{
		if ((_bodySize = std::atoi(contLen->second.c_str())) < 0)
			return ((_errorCode = 400));
		return (0);
	}
	if (trEn != headersMap.end())
	{
		if (trEn->second.find("chunked") != std::string::npos)
			_bodySize = -1;
		return ((_errorCode = 400));
	}
	return (0);
}

int			Request::parseHeaders()
{
	size_t	crlfPos;
	size_t	sepPos;
	std::string	toAnalisys;
	while (_status == firstLine && \
		(crlfPos = _buffer.find(CRLF)) != std::string::npos)
	{
		if (crlfPos == 0)
		{
			_status = headersProc;
			_buffer = _buffer.substr(crlfPos + 2);
			return (0) ; // end of headers
		}
		toAnalisys = _buffer.substr(0, crlfPos);
		sepPos = toAnalisys.find(':');
		if (sepPos == std::string::npos)
			return ((_errorCode = 400));
		std::string	key = toAnalisys.substr(0, sepPos);
		if (key.find(' ') != std::string::npos ||\
			key.find('\t') != std::string::npos)
			return ((_errorCode =  400));
		for (int i = 0; i < key.length(); i++)
			key.at(i) = std::tolower(key.at(i));
		std::string	trimmedValue;
		trimmedValue = toAnalisys.substr(sepPos + 1, crlfPos - sepPos - 1);
		trimmedValue.erase(0, trimmedValue.find_first_not_of(" \t"));
		trimmedValue.erase(trimmedValue.find_last_not_of(" \t") + 1);
		mapIter					inMap;
		inMap = headersMap.find(key);
		if ((key == "host" || key == "content-length") && inMap != headersMap.end())
			return ((_errorCode = 400));
		if (inMap != headersMap.end())
			headersMap[key] = inMap->second + ", " + trimmedValue;
		else
			headersMap[key] = trimmedValue;
		_buffer = _buffer.substr(crlfPos + 2);
	}
	return (0);
}

int			Request::validateStartLine()
{
	if (std::find(_methods.begin(), _methods.end(),\
		startLine["method"]) == _methods.end())
		return ((_errorCode = 501));
	if (startLine["version"] != "HTTP/1.1")
		return ((_errorCode = 505));
	if (startLine["uri"].length() > MAX_URI_LENGTH)
		return ((_errorCode = 414));
	if (startLine["method"] != "OPTIONS" && startLine["uri"].at(0) != '/')
		return ((_errorCode = 404));			//NOT FOUND OR BAD REQUEST? (404 vs 400)
	if (startLine["method"] == "OPTIONS" && \
		(startLine["uri"] != "*" && startLine["uri"].at(0) != '/'))
		return ((_errorCode = 400));
	return (0);
}

int			Request::parseStartLine()
{
	size_t	crlfPos = _buffer.find(CRLF);
	if (crlfPos == std::string::npos || crlfPos == 0)
		return ((_errorCode = 400));
	size_t	spacePos1 = _buffer.find(' ');
	if (spacePos1 == std::string::npos || spacePos1 > crlfPos)
		return ((_errorCode = 400));
	startLine["method"] = _buffer.substr(0, spacePos1);
	size_t	spacePos2 = _buffer.find(' ', spacePos1 + 1);
	if (spacePos2 == std::string::npos || spacePos2 > crlfPos)
		return ((_errorCode = 400));
	startLine["uri"] = _buffer.substr(spacePos1 + 1, spacePos2);
	startLine["version"] = _buffer.substr(spacePos2 + 1, crlfPos - spacePos2 - 1);
	_buffer = _buffer.substr(crlfPos + 2);
	_status = firstLine;
	if (validateStartLine())
		return (1);
	return (0);
}

Request::~Request() {}
