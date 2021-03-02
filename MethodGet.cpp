#include "MethodGet.hpp"
#include "Header.hpp"

MethodGet::~MethodGet(){ };

int			MethodGet::generateIdxPage(){
	DIR				*dir;
	struct dirent	*cur;

	dir = opendir(data.uri.script_name.c_str());//if -1
	if (!dir){//EACCES
		_statusCode = errorOpeningURL;
		return -1;//
	}
	_body = "<html>\n \
			<head><style> \
					body {background-color: rgb(252, 243, 233);}\
					h1   {color: lightseagreen;}\
					p    {color: rgb(60, 179, 113);}\
					a {color: rgba(255, 99, 71, 0.95);}\
			</style></head>\
			<body>\n"
			"<h1>Directory listing:</h1>\n";
	errno = 0;
	while ((cur = readdir(dir)) != NULL && errno == 0){
		if (cur->d_name[0] == '.')
			continue ;//
		_body += "<a href=\"" + data.uri.script_name;
		// if (path != "/")//
		// 	_body += "/";
		_body += cur->d_name;
		_body += "\">";
		_body += cur->d_name;
		_body += "</a><br>\n";
	}
	closedir(dir);
	_body += "</body>\n</html>\n";
	return 0;
}

int			MethodGet::generateErrorPage(){

	_body = "<html>\n \
			<head><style> \
					body {background-color: rgb(252, 243, 233);}\
					h1   {color: red;}\
			</style></head>\
			<body>\n"
			"<h1>ERROR ";
	_body += std::to_string(_statusCode);
	_body += "</h1>\n</body>\n</html>\n";
	return 0;
}

MethodStatus	MethodGet::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };

MethodStatus	MethodGet::manageRequest()
{
	struct stat	st;

	std::cout << "path: " << data.uri.script_name << std::endl;
	if (stat(data.uri.script_name.c_str(), &st) == -1){// && errno == ENOENT)
		std::cout << "here" << std::endl;
		_statusCode = notFound;
		generateErrorPage();//can be moved to headerCreate...
		return error;//ok;
	}

	if (S_ISDIR(st.st_mode) && data.location->autoindex){
		if (generateIdxPage())//signalize that body not from fd
			return error;//ok
	}
	
	// else if (S_ISREG(st.st_mode) && (_fd = open(data.uri.script_name.c_str(), O_RDONLY | O_NONBLOCK)) < 0){// else S_ISFIFO S_ISLNK /// O_DIRECTORY
	else if ((_fd = open(data.uri.script_name.c_str(), O_RDONLY | O_NONBLOCK)) < 0){
		_statusCode = errorOpeningURL;
		generateErrorPage();
		return error;
	}
	_statusCode = okSuccess;
	return ok;
};

// MethodStatus	MethodGet::createHeader(std::string const &path)
MethodStatus	MethodGet::createHeader()
{
	_header = new Header(data.uri.script_name);

//what headers if dir///Last Modified?
	std::cout << "\n////\tGET METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap, _statusCode);
	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206)){
		_header->createEntityHeaders(_headersMap, _statusCode);
	}
	// _header->addAllowHeader(_headersMap, _statusCode, _config);
	_header->addAllowHeader(_headersMap, _statusCode, *data.serv);
	_header->addLocationHeader(_headersMap, _statusCode);
	_header->addRetryAfterHeader(_headersMap, _statusCode);
	// _header->addTransferEncodingHeader(_headersMap, _statusCode, _headersMapRequest);
	_header->addAuthenticateHeader(_headersMap, _statusCode);

	return ok;
};

MethodStatus		MethodGet::sendResponse(int socket) { return ok; };

MethodStatus		MethodGet::sendHeader(int socket) { _statusCode = okSuccess; return ok; };

MethodStatus		MethodGet::sendBody(int socket)
{
	std::string	response;
	size_t		readBytes;
	size_t		sentBytes;
	char		buf[_bs + 1];
	size_t		readBuf = _bs;

	memset(buf, 0, _bs);
	if (_statusCode != okSendingInProgress){
		_header->headersToString(_headersMap, _statusCode, response);
		size_t headersize = response.length();

		if (!_body.empty()){//only for autoindex
			_bytesToSend = _body.length() + headersize;// - 1;//-1
			response += _body;
			size_t pos = response.find("Content-Length", 0);
			pos += 16;
			size_t fin = response.find_first_of(CRLF, pos);
			response.replace(pos, fin - pos, std::to_string(_body.length()));//to_string
		}
		else {
			struct stat sbuf;
			fstat(_fd, &sbuf);
			_bytesToSend = sbuf.st_size + headersize;
			readBuf -= headersize;
		}
		// std::cout << "response:\n" << response << "\n" << std::cout;
	}
	if (!_remainder.empty()){//only if not a full response was sent (by send)
		readBuf = _bs - _remainder.length();
		response = _remainder;
	}
	if (_body.empty()){//not listing -> //no if braces without autoindex
		readBytes = read(_fd, buf, readBuf);
		std::cout << "_________readBytes: " << readBytes << "\n" << std::cout;
		if (readBytes < 0){
			_statusCode = errorReadingURL;
			close(_fd);
			return error;
		}
		buf[readBuf] = '\0';
		std::string bufStr(buf, readBytes);
		response += bufStr;
	}

std::cout << "\t\t\t\t\t\t__________________response:\n" << response << "\n" << std::cout;
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
		return inprogress;
	};
	// _remainder.clear();//
	_sentBytesTotal += sentBytes;
	std::cout << "****\t\t\t\t\t\t\t\t_bytesToSend: " << _bytesToSend << " _sentBytesTotal: " << _sentBytesTotal << std::endl;
	if (_sentBytesTotal < _bytesToSend){
		_statusCode = okSendingInProgress;
		return inprogress;
	}

	_statusCode = okSuccess;//
	close(_fd);//do not need if error occured
	_body.clear();
	return ok;
}
