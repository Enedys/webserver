#include "Request.hpp"

Request::Request(int fd, int &statusCode) : 
_socket(fd), _errorCode(statusCode), _bodySize(0), readingStage(firstLine)
{
	createErrorCodesMap();
};

size_t				Request::getBufferResidual()
{
	return (_buffer.length());
}

MethodStatus		Request::cleanRequest()
{
	_bodySize = 0;
	readingStage = firstLine;
	startLine.clear();
	headersMap.clear();
	return (ok);
}

const stringMap		Request::getStartLine() const {return (startLine); };
const stringMap		Request::getHeadersMap() const { return (headersMap); };

std::string const	&Request::getURI() const
{
	constMapIter	uri = startLine.find("uri");
	if (uri == startLine.end())
		return ("");
	return (uri->second);
}


MethodStatus		Request::setErrorCode(int code)
{
	_errorCode = code;
	return (error);
};

MethodStatus		Request::readRequestHead(Logger *_webLogger)
{
	char	buffer[_buffer_size];
	int		readBytes = recv(_socket, buffer, _buffer_size - 1, MSG_DONTWAIT);
	std::cout << "READ " << readBytes << " bytes\n";
	if (readBytes < 0)
		return (error);
	if (readBytes > 0)
	{
		buffer[readBytes] = '\0';
		_buffer += buffer;
	}
	size_t posCRLF = _buffer.find(CRLF);
	if (posCRLF == std::string::npos)
		return (inprogress);
	if (readingStage == firstLine)
	{
		readingStage = headers;
		if (parseStartLine(posCRLF) == error)
			return (error);
	}
	if (readingStage == headers)
	{
		MethodStatus	headersStatus = parseHeaders();
		if (headersStatus == error)
			return (error);
		else if (headersStatus == ok)
			readingStage = body;
		else
			return (inprogress);
	}
	if (readingStage == body)
	{
		if (validateHeaders() == error)
			return (error);
		printRequest(); // Same logging
		return (ok);
	}
	if (readingStage < body && _buffer.length() > MAX_REQUEST_SIZE)
		return (setErrorCode(400));
	return (inprogress);
}

MethodStatus		Request::validateHeaders()
{
	if (headersMap["host"] == "")
		return (setErrorCode(400));
	mapIter	trEn = headersMap.find("transfer-encoding");
	mapIter	contLen = headersMap.find("content-length");
	if (trEn != headersMap.end() && contLen != headersMap.end())
		return (setErrorCode(400));
	if (trEn == headersMap.end() && contLen == headersMap.end())
	{
		_bodySize = 0;
		return (ok);
	}
	if (contLen != headersMap.end())
	{
		if ((_bodySize = std::atoi(contLen->second.c_str())) < 0)
			return (setErrorCode(400));
		return (ok);
	}
	if (trEn != headersMap.end())
	{
		if (trEn->second.find("chunked") != std::string::npos)
			_bodySize = -1;
		else
			return (setErrorCode(400));
	}
	return (ok);
}

MethodStatus		Request::parseHeaders()
{
	std::string	headerLine;
	size_t		sepPos;
	size_t		posCRLF = _buffer.find(CRLF);
	while (posCRLF != std::string::npos)
	{
		if (posCRLF == 0)
		{
			_buffer = _buffer.substr(posCRLF + 2);
			return (ok) ; // end of headers
		}
		headerLine = _buffer.substr(0, posCRLF);
		sepPos = headerLine.find(':');
		if (sepPos == std::string::npos)
			return (setErrorCode(400));
		std::string	key = headerLine.substr(0, sepPos);
		if (key.find(' ') != std::string::npos ||\
			key.find('\t') != std::string::npos)
			return (setErrorCode(400));
		for (int i = 0; i < key.length(); i++)
			key.at(i) = std::tolower(key.at(i));
		std::string	trimmedValue;
		trimmedValue = headerLine.substr(sepPos + 1, posCRLF - sepPos - 1);
		trimmedValue.erase(0, trimmedValue.find_first_not_of(" \t"));
		trimmedValue.erase(trimmedValue.find_last_not_of(" \t") + 1);
		mapIter	inMap = headersMap.find(key);
		if ((inMap != headersMap.end()) && (key == "host" || key == "content-length"))
			return (setErrorCode(400));
		else if (inMap != headersMap.end())
			headersMap[key] = inMap->second + ", " + trimmedValue;
		else
			headersMap[key] = trimmedValue;
		_buffer = _buffer.substr(posCRLF + 2);
		posCRLF = _buffer.find(CRLF);
	}
	return (inprogress);
}

MethodStatus		Request::validateStartLine()
{
	if (!AMethod::isValidMethod(startLine["method"]))
		return (setErrorCode(501));
	if (startLine["version"] != "HTTP/1.1")
		return (setErrorCode(505));
	if (startLine["uri"].length() > MAX_URI_LENGTH)
		return (setErrorCode(414));
	if (startLine["method"] != "OPTIONS" && startLine["uri"].at(0) != '/')
		return (setErrorCode(414));			//NOT FOUND OR BAD REQUEST? (404 vs 400)
	if (startLine["method"] == "OPTIONS" && \
		(startLine["uri"] != "*" && startLine["uri"].at(0) != '/'))
		return (setErrorCode(414));
	return (ok);
}

MethodStatus		Request::parseStartLine(size_t posCRLF)
{
	if (posCRLF == std::string::npos || posCRLF == 0)
		return (setErrorCode(400));
	size_t	spacePos1 = _buffer.find(' ');
	if (spacePos1 == std::string::npos || spacePos1 > posCRLF)
		return (setErrorCode(400));
	startLine["method"] = _buffer.substr(0, spacePos1);
	size_t	spacePos2 = _buffer.find(' ', spacePos1 + 1);
	if (spacePos2 == std::string::npos || spacePos2 > posCRLF)
		return (setErrorCode(400));
	startLine["uri"] = _buffer.substr(spacePos1 + 1, spacePos2 - spacePos1 - 1);
	startLine["version"] = _buffer.substr(spacePos2 + 1, posCRLF - spacePos2 - 1);
	_buffer = _buffer.substr(posCRLF + 2);
	if (validateStartLine() == error)
		return (error);
	return (ok);
}

MethodStatus		Request::readRequestBody(AMethod *method, Logger *_webLogger)
{
	return (ok);
}

Request::~Request() {}

/* Extra functions */
void				Request::createErrorCodesMap()
{
	_errors[400] = "Bad Request";
	_errors[404] = "Not Found";
	_errors[414] = "URI Too Long";
	_errors[501] = "Not Implemented";
	_errors[505] = "HTTP Version Not Supported";
}

/* Functions for debugging */
void				Request::printRequest()
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
