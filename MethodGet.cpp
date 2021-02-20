#include "MethodGet.hpp"
#include "Header.hpp"

MethodGet::MethodGet(t_serv const &config, int &code, stringMap const &headersMapRequest) \
	: AMethod(config, code, headersMapRequest) {
	// check_path_validity?//200
};

MethodGet::~MethodGet(){};

// for PUT POST, maybe
// updates _statusCode.
MethodStatus	MethodGet::readRequestBody(std::string const &_path) {
	size_t found;
	if ((found = path.find_last_of('.')) != std::string::npos){
		contentType = "text";//no extension
		return ok;
	}
	contentType = str.substr(found + 1);
	if (contentType == "")
	return ok;
};//+cgi

MethodStatus	MethodGet::manageRequest(std::string const &path){
	struct stat	st;
	if (stat(path.c_str(), &st) == -1){// && errno == ENOENT)//lstat?//fstat IS_DIR
		_statusCode = notFound;
		return error;
	}

	if ((_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK)) <= 0){
		_statusCode = errorOpeningURL;
		return error;
	}

	return ok;
};

MethodStatus	MethodGet::createHeader(std::string const &path) {
	Header	header(path);
	// header.setPath(path);
	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206)){ // it was not updated before
		header.createGeneralHeaders(_headersMap, _statusCode);
		header.createEntityHeaders(_headersMap, _statusCode);
		header.addLocationHeader(_headersMap, _statusCode);
	}
	else { // what if status 2xx
		header.createErrorHeader(_headersMap, _statusCode);	// with status explanation
		// sendHeader(_headersMap, _statusCode);
		return error; // means should send header and leave (without sending body)
	}
	return ok;
};

MethodStatus		MethodGet::sendHeader(int socket) {
	Header	header;
	header.headersToString(_headersMap, _statusCode, &headerStr);//// headersToString(_headersMap, &headerStr);//
	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
		//if ret < length -> loop
		_statusCode = errorSendHeader;
		return error;
	}
}

MethodStatus		MethodGet::sendBody(int socket) {
// check_socket here too?
	size_t	ret;
	char	buf[BUFSIZE];

// // addContentLengthHeader();//Entity //+path/
// 	std::string headerStr;
// 	Header	header;
// 	header.headersToString(_headersMap, _statusCode, &headerStr);//// headersToString(_headersMap, &headerStr);//
// 	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
// 		//if ret < length -> loop
// 		_statusCode = errorSendHeader;
// 		return error;
// 	}

	while ((ret = read(_fd, buf, BUFSIZE)) >= 0){
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



// MethodStatus		MethodGet::sendResponse(int socket) {
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



// MethodStatus		MethodGet::sendResponse(int socket) {
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