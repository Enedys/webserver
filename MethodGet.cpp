#include "MethodGet.hpp"
#include "Header.hpp"
#include "OutputConfigurator.hpp"

MethodGet::~MethodGet(){ delete _header; };
MethodStatus		MethodGet::sendResponse(int socket) { return ok; };
MethodStatus		MethodGet::sendHeader(int socket) { return ok; };

MethodStatus	MethodGet::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };

// POST || .php .py (cgi in config) || GET HEAD + no autoindex + IS_DIR + stat(index.php) in config - manageRequest
//is error and error_page is set ad cgi extension - create header

MethodStatus	MethodGet::manageRequest()
{
	if (_statusCode != 0)//по старой логике
		return ok;

	if (_bodyType == bodyIsTextErrorPage)
		_body = generateErrorPage();
	else if (_bodyType == bodyIsAutoindex)
		_body = generateIdxPage();

	if (_bodyType == bodyIsCGI)
	{
		_statusCode = cgi.init(data);
	}
	// else // это мб у Дани
	// {
	// 	_fd = open(data.uri.script_name.c_str(), O_RDONLY | O_NONBLOCK);
	// 	if (_fd < 0)
	// 		_statusCode = errorOpeningURL;
	// }

	return ok;
};

MethodStatus	MethodGet::createHeader()//createResponse()
{
	if (_bodyType == bodyIsCGI){
		// cgi.smartOutput(_body);
		return ok;
	}

	_header = new Header(data.uri.script_name, data.location->root, _statusCode);//get rid of

	std::cout << "\n////\tGET METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap);

	// if (_statusCode < 200 || _statusCode > 206)
	// 	_header->generateErrorPage(_body, data.serv->error_pages);
	// else if (_body.length() == 0)///
	_header->addContentLengthHeader(_headersMap, _body);//for GET//body for auto+error//if not dir!

	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206))
		_header->createEntityHeaders(_headersMap);
	if (_statusCode == 405)
		_header->addAllowHeader(_headersMap, *data.location);
	_header->addLocationHeader(_headersMap);//if redirect
	_header->addRetryAfterHeader(_headersMap);//503 429
	// _header->addTransferEncodingHeader(_headersMap, _headersMapRequest);
	_header->addAuthenticateHeader(_headersMap);

	std::string headerStr = _header->headersToString(_headersMap);
	_body.insert(0, headerStr);
	delete _header;

	return ok;
};

MethodStatus		MethodGet::sendBody(int socket)
{
	std::string	response;
	size_t		readBytes;
	size_t		sentBytes;
	char		buf[_bs + 1];
	size_t		readBuf = _bs;

	memset(buf, 0, _bs);
	if (_statusCode != okSendingInProgress)//do not update _statusCode
	{
		if (_bodyType == bodyIsFile)
		{
			struct stat sbuf;
			fstat(_fd, &sbuf);
			_bytesToSend = _body.length() + sbuf.st_size;
			readBuf -= _body.length();
		}
		else
			_bytesToSend = _body.length();
	}

	if (!_remainder.empty()){//only if not a full response was sent (by send)
		// if (_bodyType == bodyIsCGI)
		// {
		// 	response += _remainder;
		// }
		// else
		// {
			readBuf = _bs - _remainder.length();
			response = _remainder;
		// }
	}

	if (_bodyType == bodyIsFile)
	{
		readBytes = read(_fd, buf, readBuf);	//configure _fd before!
		if (readBytes < 0){
			_statusCode = errorReadingURL;
			close(_fd);
			return error;//what happens after, a_body.length();bove?
		}
		buf[readBuf] = '\0';
		std::string bufStr(buf, readBytes);
		response += bufStr;
	}
	else if (_bodyType == bodyIsCGI)
	{
		// _statusCode = cgi.smartOutput(response);
		// if (!_remainder.empty())
		// 	response.insert(0, _remainder);//overflow velocity?
		// // _bytesToSend = response.length();

		if (_remainder.empty())
			_statusCode = cgi.smartOutput(response);
		// _bytesToSend = response.length();
	}


	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || errno == EMSGSIZE){
		_statusCode = errorSendingResponse;
		close(_fd);
		return error;
	}
	if (sentBytes < response.length()){
		_remainder.assign(response.c_str(), sentBytes, response.length() - sentBytes);
		_sentBytesTotal += sentBytes;
		_statusCode = okSendingInProgress;
		return inprogress;//cgi
	};
	// _remainder.clear();//
	if (_bodyType == bodyIsCGI && _statusCode == ok)
		return ok;

	_sentBytesTotal += sentBytes;//
	if (_sentBytesTotal < _bytesToSend){
		_statusCode = okSendingInProgress;
		return inprogress;
	}

	close(_fd);
	return ok;
}

