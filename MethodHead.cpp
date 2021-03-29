#include "MethodHead.hpp"
#include "Header.hpp"

MethodHead::~MethodHead() { delete _header; };
MethodStatus 	MethodHead::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodHead::sendResponse(int socket) { return (ok); };
MethodStatus	MethodHead::sendHeader(int socket) { return (ok); };

MethodStatus	MethodHead::manageRequest() {

	struct stat	st;

	_statusCode = okSuccess;
	if (stat(data.uri.script_name.c_str(), &st) == -1){// && errno == ENOENT)
		_statusCode = notFound;
		return error;
	}
	if ((_fd = open(data.uri.script_name.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
		_statusCode = errorOpeningURL;
	return ok;
};

MethodStatus	MethodHead::createHeader()
{
	// _header = new Header(data.uri.script_name, _statusCode);
	_header = new Header(data.uri.script_name, data.location->root, _statusCode);

	std::cout << "////\t\t HEAD METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap);

	if (_statusCode < 200 || _statusCode > 206)
		_header->generateErrorPage(_body, data.serv->error_pages);
	else//if no autoindex
		_header->addContentLengthHeader(_headersMap, _body);//for GET//body for auto+error//if not dir!

	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206))
		_header->createEntityHeaders(_headersMap);

	// _header->addContentLengthHeader(_headersMap, _body);//if not dir!
	if (_statusCode == 405)
		_header->addAllowHeader(_headersMap, *data.location);
	_header->addLocationHeader(_headersMap);
	_header->addRetryAfterHeader(_headersMap);
	// _header->addTransferEncodingHeader(_headersMap, _headersMapRequest);
	_header->addAuthenticateHeader(_headersMap);

	return ok;
};

MethodStatus		MethodHead::sendBody(int socket)
{
	std::string	response;
	size_t		sentBytes;

	if (_remainder.length())
		response = _remainder;
	if (_statusCode != okSendingInProgress)
		_header->headersToString(_headersMap, response);

std::cout << "__________response: " << response << "\n" << std::endl;
	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || errno == EMSGSIZE){
		_statusCode = errorSendingResponse;
		close(_fd);
		return error;
	}
	if (sentBytes < response.length()){
		_remainder.assign(response.c_str(), sentBytes, response.length() - sentBytes);
		_statusCode = okSendingInProgress;
		return inprogress;
	};
	_remainder.clear();
	close(_fd);
	return ok;
}
