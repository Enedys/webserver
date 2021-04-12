#include "MethodPut.hpp"

MethodPut::~MethodPut() {}

MethodStatus	MethodPut::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	if (_statusCode == 403 || _statusCode == 405)
		return ok;

	_fd = open(data.uri.script_name.c_str(), O_WRONLY | O_APPEND | O_CREAT| O_NONBLOCK, 0644);
	if (_fd < 0){
		_statusCode = 404;
		return error;
	}

	size_t res = write(_fd, requestBody.c_str(), requestBody.length());
	close(_fd);
	if (res < requestBody.length()){
		_statusCode = 404;
		return error;
	}

	_statusCode = _statusCode == 404 ? 201 : 204;

	if (bodyStatus == inprogress)
		return inprogress;
	return ok;
}

MethodStatus	MethodPut::manageRequest()
{
	if (!data.location->putAvailable)
		_statusCode = 405;

	else if (_bodyType == bodyIsAutoindex)
		_statusCode = 403;

	_bodyType = bodyIsEmpty;

	return ok;
}
