#include "MethodGet.hpp"
#include "Header.hpp"

# define BUFSIZE 4096

MethodGet::MethodGet(t_serv const &config, int &code, stringMap const &headers) \
	: AMethod(config, code, headers) { _sentBytesTotal = 0; };

MethodGet::~MethodGet(){ };

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
	_header->headersToString(_headersMap, _statusCode, headerStr);//// headersToString(_headersMap, &headerStr);//
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
	char	buf[BUFSIZE];

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

MethodStatus		MethodGet::sendResponse(int socket)
{
	std::string	response;
	size_t		readBytes = 0;
	size_t		sentBytes;
	char		buf[BUFSIZE + 1];
	size_t		headersize = 0;

	struct stat sbuf;
	int res = fstat(_fd, &sbuf);
	size_t filesize = sbuf.st_size;
	std::cout << "file size:\t" << filesize << std::endl;

	memset(buf, 0, BUFSIZE);
	if (_statusCode == okSendingInProgress){
		readBytes = read(_fd, buf, BUFSIZE);
		buf[BUFSIZE] = '\0';
		std::string bufStr(buf);
		response += bufStr;

		std::cout << "readBytes:\t" << readBytes << "\tresponse l:\t" << response.length() << std::endl;
	}
	else {
		_header->headersToString(_headersMap, _statusCode, response);
		headersize = response.length();
		std::cout << "header size:\t" << headersize << "\tresp length:\t" << response.length() << std::endl;

		readBytes = read(_fd, buf, BUFSIZE - headersize);//buf > hlength//-1
		buf[BUFSIZE - headersize] = '\0';
		std::string bufStr(buf);
		response += bufStr;
		std::cout << "readBytes:\t" << readBytes << "\nh + b length:\t" << response.length() << std::endl;
	}
	if (readBytes < 0){
		std::cout << "readBytes < 0" << std::endl;
		_statusCode = errorSendingResponse;
		close(_fd);
		return error;
	}
	else if (readBytes == 0){// && _sentBytesTotal >= filesize){// && response.find(EOF)){//&& sentTotal == filesize + headersize
		std::cout << "finished reading" << std::endl;
		close(_fd);
		return ok;
	}


	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || errno == EMSGSIZE){
		std::cout << "error sending" << std::endl;
		_statusCode = errorSendingResponse;
		close(_fd);
		return error;
	}
	// if (sentBytes < response.length()){
		// remainder =
	// };
	_sentBytesTotal += sentBytes;
	if (_sentBytesTotal < filesize + headersize){//&& is not the end of file
		std::cout << "sending in progress" << std::endl;
		_statusCode = okSendingInProgress;
		return inprogress;
	}
	return ok;
}