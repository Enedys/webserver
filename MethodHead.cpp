#include "MethodHead.hpp"
#include "Header.hpp"

// MethodHead::~MethodHead() {};
// MethodStatus	MethodHead::createHeader() { return (ok); };
// MethodStatus	MethodHead::readRequestBody(int socket) { return (ok); };
// MethodStatus	MethodHead::manageRequest(std::string const &path) { return (ok); };
// MethodStatus	MethodHead::sendBody(int socket) { return (ok); };
// MethodStatus	MethodHead::sendHeader(int socket)
// {
// 	return (AMethod::sendHeader(socket));
// };

MethodHead::MethodHead(t_serv const &config, int &code, stringMap const &headersMapRequest) \
	: AMethod(config, code, headersMapRequest) {
	// check_path_validity?//200
};

MethodHead::~MethodHead(){};

// for PUT POST, maybe
// updates _statusCode.
MethodStatus	MethodHead::readRequestBody() { return ok; };

MethodStatus	MethodHead::manageRequest(std::string const &path){
	struct stat	st;
	if (stat(path.c_str(), &st) == -1){// && errno == ENOENT){//lstat?//fstat IS_DIR
		_statusCode = notFound;
		return error;
	}

	if ((_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK)) <= 0){
		_statusCode = errorOpeningURL;
		return error;
	}

	return ok;
};

MethodStatus	MethodHead::createHeader() {
	Header	header;

	header.createGeneralHeaders(_headersMap, _statusCode);
	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206)){ // it was not updated before
		header.createEntityHeaders(_headersMap, _statusCode);
		header.addLocationHeader(_headersMap, _statusCode);//add path, if redirects - default path
	}
	else { // what if status 2xx
		header.createErrorHeader(_headersMap, _statusCode);	// with status explanation
		// sendHeader(_headersMap, _statusCode);
		return error; // means should send header and leave (without sending body)
	}
	return ok;
};

MethodStatus		MethodHead::sendHeader(int socket) {
	std::string headerStr;
	// Header	header;
	// header.headersToString(_headersMap, &_statusCode, &headerStr);//

	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
		//if ret < length -> loop
		_statusCode = errorSendHeader;
		return error;
	}

	return ok;
};

MethodStatus		MethodHead::sendBody(int socket) {
// check_socket here too?
	size_t	ret;
	char	buf[BUFSIZE];

// addContentLengthHeader();//Entity //+path/
	while ((ret = read(_fd, buf, BUFSIZE)) >= 0){
		int sent = write(socket, buf, ret);
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
