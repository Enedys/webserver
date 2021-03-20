#include "MethodPut.hpp"

// MethodPut::MethodPut(t_serv const &config, int &code, stringMap const &headers) : AMethod(config, code, headers) {};
MethodPut::~MethodPut() {};
MethodStatus	MethodPut::createHeader()
{
	_header = new Header(data.uri.script_name, data.location->root, _statusCode);

	std::cout << "////\t\t PUT METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap);

	if (_statusCode < 200 || _statusCode > 206)
		_header->generateErrorPage(_body, data.serv->error_pages);
	// else//if no autoindex
	// 	_header->addContentLengthHeader(_headersMap, _body);
	_headersMap.insert(std::pair<std::string, std::string>("Content-Length", "0"));
	// if (_statusCode == 405)
	// 	_header->addAllowHeader(_headersMap, *data.serv);
	return ok;
};

MethodStatus	MethodPut::processBody(const std::string &requestBody, MethodStatus bodyStatus) {
 //if status == ok, write return ok
	// if (bodyStatus == inprogress){ //max_body
		// receive more body in the next iteration
		// _statusCode = ///;//
		//}

	// if (!data.serv->locs[0].optionsAvailable)//locs[i]
	if (!data.location->optionsAvailable)
		_statusCode = methodNotAllowed;

	struct stat	st;
	errno = 0;//S_ISDIR
	_statusCode = notFound;///EACCES
	_fd = open(data.uri.script_name.c_str(), O_WRONLY | O_APPEND | O_CREAT| O_NONBLOCK, 0644);
	if (_fd < 0){
		_statusCode = notFound;
		return ok;//error
	}

	int res = write(_fd, requestBody.c_str(), requestBody.length());
	if (res < requestBody.length())
		_statusCode = notFound;//internalError;
	// if (stat(data.uri.script_name.c_str(), &st) == -1){// файла нет
	// 	if (errno == ENOENT)
	// 		fd = open(data.uri.script_name.c_str(), O_WRONLY, 0644);//O_APPEND O_CREAT
	// }
	// else

	// if ((fd = open(data.uri.script_name.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
	// // 	_statusCode = errorOpeningURL;
	
	
	// if status == ok or later close(_fd);//if not in progress
	_statusCode = created;//201
	return (ok);//or inprogress
};

MethodStatus	MethodPut::manageRequest() { return (ok); };

MethodStatus	MethodPut::sendBody(int socket) {
	std::string	response;
	size_t		sentBytes;

	if (_remainder.length())
		response = _remainder;
	if (_statusCode != okSendingInProgress)
		_header->headersToString(_headersMap, response);

	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || errno == EMSGSIZE){
		_statusCode = errorSendingResponse;
		// close(_fd);
		return error;
	}
	if (sentBytes < response.length()){
		_remainder.assign(response.c_str(), sentBytes, response.length() - sentBytes);
		_statusCode = okSendingInProgress;
		return inprogress;
	};
	// _remainder.clear();
	// close(_fd);
	return ok;
 };

MethodStatus	MethodPut::sendHeader(int socket){ return ok; };

MethodStatus	MethodPut::sendResponse(int socket){ return ok; };