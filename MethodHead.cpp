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
	if ((_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK)) <= 0){//
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
	// _header->addTransferEncodingHeader(_headersMap, _statusCode, _headersMapRequest);
	_header->addAuthenticateHeader(_headersMap, _statusCode);

	return ok;
};

MethodStatus		MethodHead::sendHeader(int socket) {
	std::string headerStr;
	_header->headersToString(_headersMap, _statusCode, &headerStr);//// headersToString(_headersMap, &headerStr);//
	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){
		//if ret < length -> loop
		_statusCode = errorSendingResponse;
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

MethodStatus		MethodHead::sendResponse(int socket) {
	std::string	response;
	size_t		sentBytes;

	_header->headersToString(_headersMap, _statusCode, &response);
	close(_fd);
	std::cout << "Response header string: \n" << response <<std::endl;

	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	std::cout << "sentBytes: " << sentBytes <<std::endl;
	if (sentBytes < 0 || sentBytes == EMSGSIZE){
		_statusCode = errorSendingResponse;
		return error;
	}
	if (sentBytes < response.length())
	{
		_statusCode = okSendingInProgress;//okSuccess;
		return inprogress;
	}
	return ok;
}
