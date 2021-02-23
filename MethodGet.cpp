#include "MethodGet.hpp"
#include "Header.hpp"

MethodGet::MethodGet(t_serv const &config, int &code, stringMap const &headers) \
	: AMethod(config, code, headers) { _sentBytesTotal = 0; };

MethodGet::~MethodGet(){ };

MethodStatus	MethodGet::readRequestBody(int socket) { return ok; };

int			MethodGet::generateIdxPage(std::string const &path){
	DIR				*dir;
	struct dirent	*cur;

	dir = opendir(path.c_str());//if -1
	if (!dir){//EACCES
		_statusCode = errorOpeningURL;
		return -1;//
	}
	_body = "<html>\n<body>\n"
			"<h1>Directory listing:</h1>\n";
	errno = 0;
	while ((cur = readdir(dir)) != NULL && errno == 0){
		if (cur->d_name[0] == '.')
			continue ;//
		_body += "<a href=\"" + path;
		if (path != "/")
			_body += "/";
		_body += cur->d_name;
		_body += "\">";
		_body += cur->d_name;
		_body += "</a><br>\n";
		// std::cout << "_body" << _body << std::endl;
	}
	closedir(dir);
	_body += "</body>\n</html>\n";
	return 0;
}

MethodStatus	MethodGet::manageRequest(std::string const &path)
{
	struct stat	st;
	if (stat(path.c_str(), &st) == -1){// && errno == ENOENT)
		_statusCode = notFound;
		return error;
	}
	if (S_ISDIR(st.st_mode) && _config.locs[0].autoindex){
		if (generateIdxPage(path))//signalize that body not from fd
			return error;
	}
	// else S_ISFIFO S_ISLNK /// O_DIRECTORY
	else if (S_ISREG(st.st_mode) && (_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK)) < 0){
		_statusCode = errorOpeningURL;
		return error;
	}
	_statusCode = okSuccess;
	return ok;
};

MethodStatus	MethodGet::createHeader(std::string const &path)
{
	_header = new Header(path);

//what headers if dir///Last Modified?
	std::cout << "////\tGET METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap, _statusCode);
	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206)){
		_header->createEntityHeaders(_headersMap, _statusCode);
	}
	_header->addAllowHeader(_headersMap, _statusCode, _config);
	_header->addLocationHeader(_headersMap, _statusCode);
	_header->addRetryAfterHeader(_headersMap, _statusCode);
	// _header->addTransferEncodingHeader(_headersMap, _statusCode, _headersMapRequest);
	_header->addAuthenticateHeader(_headersMap, _statusCode);

	return ok;
};

MethodStatus		MethodGet::sendHeader(int socket)
{
	std::string headerStr;
	_header->headersToString(_headersMap, _statusCode, headerStr);
	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
		//if ret < length -> loop
		_statusCode = errorSendingResponse;
		return error;
	}
	std::cout << "Response header string: \n" << headerStr <<std::endl;
	return ok;
}

MethodStatus		MethodGet::sendBody(int socket)
{
	size_t	ret;
	char	buf[_bs];

	while ((ret = read(_fd, buf, _bs)) >= 0){
		size_t sent = write(socket, buf, ret);
		if (sent == 0)
			break ;
		if (sent < 0){
			_statusCode = errorSocket;
			return error;
		}
	}
	if (ret == -1){
		_statusCode = errorReadingURL;//updates STATUSCODE or return error
		return error;
	}
	close(_fd);
	_statusCode = okSuccess;
	return ok;
}

MethodStatus		MethodGet::sendResponse(int socket)
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
			_bytesToSend = _body.length() + headersize - 1;//-1
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
	}
	if (!_remainder.empty()){
		readBuf = _bs - _remainder.length();
		response = _remainder;
	}
	if (_body.empty()){//not listing -> //no if braces without autoindex
		readBytes = read(_fd, buf, readBuf);
		if (readBytes < 0){
			_statusCode = errorReadingURL;
			close(_fd);
			return error;
		}
		buf[readBuf] = '\0';
		std::string bufStr(buf, readBytes);
		response += bufStr;
	}

	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || errno == EMSGSIZE){
		_statusCode = errorSendingResponse;
		close(_fd);
		return error;
	}
	if (sentBytes < response.length()){
		// char const *str = response.c_str();
		// str += sentBytes;
		// _remainder.assign(str);
		_remainder.assign(response.c_str(), sentBytes, response.length() - sentBytes);
		_sentBytesTotal += sentBytes;
		_statusCode = okSendingInProgress;
		return inprogress;
	};
	// _remainder.clear();//
	_sentBytesTotal += sentBytes;
	std::cout << "_bytesToSend" << _bytesToSend << " _sentBytesTotal" << _sentBytesTotal << std::endl;
	if (_sentBytesTotal < _bytesToSend){
		_statusCode = okSendingInProgress;
		return inprogress;
	}
	close(_fd);
	return ok;
}
