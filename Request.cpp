#include "Request.hpp"

Request::Request(int fd, int &statusCode) :
requestStage(firstLine), _socket(fd), _bodySize(0), _errorCode(statusCode)
{
	end_body = false;
	chunkSize = 0;
	trEnSize = 0;
	residBodysize = 0;
	createErrorCodesMap();
}

size_t				Request::getBufferResidual()
{
	return (_buffer.length());
}

Request::requestStatus		
					Request::getRequestState() const
{
	return (requestStage);
}

MethodStatus		Request::cleanRequest()
{
	end_body = false;
	chunkSize = 0;
	trEnSize = 0;
	_bodySize = 0;
	residBodysize = 0;
	requestStage = firstLine;
	startLine.clear();
	headersMap.clear();
	return (ok);
}

const stringMap		&Request::getStartLine() const {return (startLine); }
const stringMap		&Request::getHeadersMap() const { return (headersMap); }

std::string const	&Request::getURI()
{
	constMapIter	uri = startLine.find("uri");
	if (uri == startLine.end())
		startLine["uri"] = "";
	return (startLine["uri"]);
}

std::string const	&Request::getMethod()
{
	constMapIter	uri = startLine.find("method");
	if (uri == startLine.end())
		startLine["method"] = "";
	return (startLine["method"]);
}

size_t				Request::getContentLength()
{
	return (_bodySize);
}

MethodStatus		Request::setErrorCode(int code)
{
	_errorCode = code;
	return (error);
}

void				Request::setBodyLimit(size_t lim)
{
	_bodyLimit = lim;
	residBodysize = _bodySize;
}

MethodStatus		Request::getRequestHead()
{
	std::cout << "\033[32m Into getRequestHead: " << requestStage << " IN BUFFER: "\
			<< getBufferResidual() << "\033[0m "<< std::endl;
	if (requestStage < body && _buffer.length() > MAX_REQUEST_SIZE)
		return (setErrorCode(400));
	size_t posCRLF = _buffer.find(CRLF);
	if (posCRLF == std::string::npos)
		return (inprogress);
	MethodStatus	headStatus = parseRequestHead(posCRLF);
	if (headStatus == error)
		return (error);
	return (headStatus);
}

MethodStatus		Request::readFromSocket()
{
	size_t const	bufsize = requestStage < body ? _headBufsize : _bodyBufsize;
	char	buffer[bufsize + 1];
	int		readBytes = recv(_socket, buffer, bufsize, MSG_DONTWAIT);
	std::cout << "READ " << readBytes << " bytes fromsocket: " << _socket << "\n";
	if (readBytes < 0)
		return (connectionClosed);
	else if (readBytes == 0)
		return (connectionClosed);
	buffer[readBytes] = '\0';
	_buffer += buffer;
	//std::cout << "INBUFFER: " << _buffer << '\n';
	return (inprogress);
}

MethodStatus		Request::parseRequestHead(size_t posCRLF)
{
	if (requestStage == firstLine)
	{
		requestStage = headers;
		if (parseStartLine(posCRLF) == error)
			return (error);
	}
	if (requestStage == headers)
	{
		MethodStatus	headersStatus = parseHeaders();
		if (headersStatus == error)
			return (error);
		else if (headersStatus == ok)
			requestStage = body;
		else
			return (inprogress);
	}
	if (requestStage == body)
	{
		if (validateHeaders() == error)
			return (error);
		printRequest();
	}
	return (ok);
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
			return (ok); // end of headers
		}
		headerLine = _buffer.substr(0, posCRLF);
		sepPos = headerLine.find(':');
		if (sepPos == std::string::npos)
			return (setErrorCode(400));
		std::string	key = headerLine.substr(0, sepPos);
		if (key.find(' ') != std::string::npos ||\
			key.find('\t') != std::string::npos)
			return (setErrorCode(400));
		for (size_t i = 0; i < key.length(); i++)
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
{//added delete, check if delete changes smth in current logic
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

MethodStatus		Request::getRequestBody(AMethod *method)
{
	if (_bodySize > (int)_bodyLimit)
		return (setErrorCode(413));
	MethodStatus	rbodyStatus = ok;
	std::string		reqBody;
	if (requestStage != body)
		return (logicError);
	if (residBodysize == 0)
		residBodysize = _bodySize;
	if (_bodySize > 0)
	{
		reqBody = _buffer.substr(0, residBodysize);
		residBodysize -= reqBody.length();
		_buffer = _buffer.erase(0, reqBody.length());
		rbodyStatus = residBodysize == 0 ? ok : inprogress;
	}
	else if (_bodySize < 0)
	{
		rbodyStatus = getTrEncodedMsg(reqBody);
		trEnSize += reqBody.size();
		if (trEnSize > (int)_bodyLimit)
			return (setErrorCode(413));
		if (rbodyStatus == ok)
			trEnSize = 0;
	}
	if (rbodyStatus == error)
		return (error);
	if (rbodyStatus == ok) 		// What reason to change requestStage?
		requestStage = init; 	// Answer: stop transfering to pipe after end of the msg bodey
	rbodyStatus = method->processBody(reqBody, rbodyStatus);
	return (rbodyStatus);
}

MethodStatus	Request::getTrEncodedMsg(std::string &dest)
{
	dest.clear();
	size_t			pullBytes = 0;
	size_t			posCRLF;
	while (true)
	{
		posCRLF = _buffer.find(CRLF, pullBytes);
		if (chunkSize)
		{
			std::string	chunk_part = _buffer.substr(pullBytes, chunkSize - 2); 
			dest += chunk_part;
			chunkSize -= chunk_part.length();
			pullBytes += chunk_part.length();
			if (posCRLF != std::string::npos && chunkSize != 2)
				return (setErrorCode(418));
			else if (posCRLF == std::string::npos)
			{
				_buffer.erase(0, pullBytes);
				return (inprogress);
			}
			pullBytes += 2;
			chunkSize = 0;
			continue ;
		}
		if (end_body)
			break ;
		if (posCRLF == std::string::npos)
		{
			if (pullBytes > 0)
				_buffer.erase(0, pullBytes);
			return (inprogress);
		}
		chunkSize = string2Size(_buffer.substr(pullBytes, posCRLF - pullBytes)) + 2;
		if (chunkSize < 2)
			return (setErrorCode(418));
		else if (chunkSize == 2)
			end_body = true;
		pullBytes += (posCRLF - pullBytes + 2);
	}
	if (pullBytes > 0)
		_buffer.erase(0, pullBytes);
	chunkSize = 0;
	end_body = false;
	return (ok);
}

Request::~Request()
{
}

/* Extra functions */
void				Request::createErrorCodesMap()
{
	_errors[400] = "Bad Request";
	_errors[404] = "Not Found";
	_errors[414] = "URI Too Long";
	_errors[500] = "Internal Server Error";
	_errors[501] = "Not Implemented";
	_errors[505] = "HTTP Version Not Supported";
	_errors[507] = "Insufficient Storage";
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
