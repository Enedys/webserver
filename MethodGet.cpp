#include "MethodGet.hpp"
#include "Header.hpp"

MethodGet::MethodGet(t_serv const &config, int &code, stringMap const &headers) \
	: AMethod(config, code, headers) {};

MethodGet::~MethodGet(){};

MethodStatus	MethodGet::readRequestBody(int socket) { return ok; };

MethodStatus	MethodGet::manageRequest(std::string const &path)
{
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

MethodStatus	MethodGet::createHeader(std::string const &path)
{
	_header = new Header(path);

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

MethodStatus		MethodGet::sendHeader(int socket) {
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


MethodStatus		MethodGet::sendResponse(int socket) {
	std::string	response;
	size_t		readBytes;
	size_t		sentBytes;
	char		buf[BUFSIZE];

	_header->headersToString(_headersMap, _statusCode, &response);
	std::cout << "Response header string: \n" << response <<std::endl;
	readBytes = read(_fd, buf, BUFSIZE);
	close(_fd);
	std::string bufStr(buf);
	response += bufStr;

	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || sentBytes == EMSGSIZE)
		return error;
	if (sentBytes < response.length())
	{
		_statusCode = okSendingInProgress;//okSuccess;
		return inprogress;
	}
	return ok;
}
