#include "MethodHead.hpp"
#include "Header.hpp"

MethodHead::MethodHead(t_serv const &config, int &code, stringMap const &headers) \
	: AMethod(config, code, headers) {};

MethodHead::~MethodHead(){};

MethodStatus	MethodHead::readRequestBody(int socket) { return ok; };

MethodStatus	MethodHead::manageRequest(std::string const &path){
	struct stat	st;
	if (stat(path.c_str(), &st) == -1){// && errno == ENOENT)//lstat?//fstat IS_DIR
		_statusCode = notFound;
		return error;
	}
	if ((_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK)) <= 0){
		_statusCode = errorOpeningURL;
		return error;
	}
	_statusCode = okSuccess;
	return ok;
};

MethodStatus	MethodHead::createHeader(std::string const &path)
{
	_header = new Header(path);

	std::cout << "////\t\t HEAD METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap, _statusCode);
	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206)){
		_header->createEntityHeaders(_headersMap, _statusCode);
	}
	_header->addAllowHeader(_headersMap, _statusCode, _config);
	_header->addLocationHeader(_headersMap, _statusCode);
	_header->addRetryAfterHeader(_headersMap, _statusCode);
	_header->addTransferEncodingHeader(_headersMap, _statusCode);
	_header->addAuthenticateHeader(_headersMap, _statusCode);

	return ok;
};

MethodStatus		MethodHead::sendHeader(int socket) {
	std::string headerStr;
	_header->headersToString(_headersMap, _statusCode, &headerStr);//// headersToString(_headersMap, &headerStr);//
	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
		//if ret < length -> loop
		_statusCode = errorSendHeader;
		return error;
	}
	std::cout << "Response header string: \n" << headerStr <<std::endl;
	return ok;
}

MethodStatus		MethodHead::sendBody(int socket)
{
	close(_fd);
	_statusCode = okSuccess;
	return ok;
}


// MethodStatus		MethodHead::sendResponse(int socket) {
// 	std::string	response;
// 	size_t		readBytes;
// 	char		buf[BUFSIZE];
// 	std::string	headerStr;

// 	_header->headersToString(_headersMap, _statusCode, &headerStr);
// 	response += headerStr;
// 	// readBytes = read(_fd, buf, BUFSIZE - headerStr.length());
// 	readBytes = read(_fd, buf, BUFSIZE);
// 	response +=
// }
// MethodStatus		MethodHead::sendResponse(int socket) {
// // check_socket here too?
// 	size_t	ret;
// 	char	buf[BUFSIZE];

// // addContentLengthHeader();//Entity //+path/
// 	std::string headerStr;
// 	Header	header;
// 	header.headersToString(_headersMap, _statusCode, &headerStr);//// headersToString(_headersMap, &headerStr);//
// 	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
// 		//if ret < length -> loop
// 		_statusCode = errorSendHeader;
// 		return error;
// 	}

// 	char *rem = buf;
// 	while ((ret = read(_fd, buf, BUFSIZE)) >= 0){// >= 0 //if 0
// 		size_t sent = 0;
// 		size_t length = ret < BUFSIZE ? ret : BUFSIZE; // EMSGSIZE
// 		// while ((sent = send(socket, rem, length, 0)) >= 0){
// 		while ((sent = write(socket, rem, length)) >= 0){ //, O_NONBLOCK);))
// 			if (sent == length)
// 				break ;
// 			else {
// 				rem += sent;
// 				length -= sent;
// 			}
// 		}
// 		if (sent == 0)//
// 			break ;
// 		if (sent < 0){// 	if (errno == ENOTSOCK)	// 		std::cout << "socket error\n" << std::endl;
// 			std::cout << "error\n" << std::endl;
// 			// _status = errorSocket;
// 			return error;
// 		}
// 	}
// 	if (ret == -1){
// 		// _status = errorReadingURL;
// 		return error;
// 	}
// 	close(_fd);
// 	_statusCode = okSuccess;
// 	return ok;
// }



// MethodStatus		MethodHead::sendResponse(int socket) {
// 	size_t	ret;
// 	char	buf[BUFSIZE];

// // addContentLengthHeader();//Entity //+path/
// 	std::string headerStr;
// 	Header	header;
// 	//auth, Authorization, listing
// 	header.headersToString(_headersMap, _statusCode, &headerStr);//// headersToString(_headersMap, &headerStr);//
// 	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
// 		//if ret < length -> loop
// 		_statusCode = errorSendHeader;
// 		return error;
// 	}

// 	char *rem = buf;
// 	while ((ret = read(_fd, buf, BUFSIZE)) >= 0){// >= 0 //if 0
// 		size_t sent = 0;
// 		size_t length = ret < BUFSIZE ? ret : BUFSIZE; // EMSGSIZE
// 		while ((sent = send(socket, rem, length, 0)) >= 0){ // while ((sent = write(socket, rem, length)) >= 0){ //, O_NONBLOCK);))
// 			if (sent == length)
// 				break ;
// 			else {
// 				rem += sent;
// 				length -= sent;
// 			}
// 		}
// 		if (sent == 0)//
// 			break ;
// 		if (sent < 0){// 	if (errno == ENOTSOCK)	// 		std::cout << "socket error\n" << std::endl;
// 			std::cout << "error\n" << std::endl;
// 			// _status = errorSocket;
// 			return error;
// 		}
// 	}
// 	if (ret == -1){
// 		// _status = errorReadingURL;
// 		return error;
// 	}
// 	close(_fd);
// 	_statusCode = okSuccess;
// 	return ok;
// }

// MethodHead::~MethodHead() {};
// MethodStatus	MethodHead::createHeader() { return (ok); };
// MethodStatus	MethodHead::readRequestBody(int socket) { return (ok); };
// MethodStatus	MethodHead::manageRequest(std::string const &path) { return (ok); };
// MethodStatus	MethodHead::sendBody(int socket) { return (ok); };
// MethodStatus	MethodHead::sendHeader(int socket)
// {
// 	return (AMethod::sendHeader(socket));
// }
