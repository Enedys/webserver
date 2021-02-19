# include "MethodGet.hpp"
// # include "TestMethod.hpp"

MethodGet::MethodGet(t_serv const &config, int &code, stringMap const &headersMapRequest) \
	: AMethod(config, code, headersMapRequest) {
	// check_path_validity?//200
};

MethodGet::~MethodGet(){};

// for PUT POST, maybe
// updates _statusCode.
MethodStatus	MethodGet::readRequestBody() { return ok; };

MethodStatus	MethodGet::manageRequest(std::string const &path){
	struct stat	st;
	if (stat(path.c_str(), &st) == -1){//lstat?//fstat IS_DIR
		_statusCode = notFound;
		return error;
	}

	if ((_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK)) <= 0){
		_statusCode = errorOpeningURL;
		return error;
	}

	return ok;
};

MethodStatus	MethodGet::createHeader() {
	Header	header;
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
	std::string headerStr;
	mapToString(_headersMap, &headerStr);//

	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
		//if ret < length -> loop
		_statusCode = errorSendHeader;
		return error;
	}

	return ok;
};

MethodStatus		MethodGet::sendBody(int socket) {
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
