#include "MethodPut.hpp"

MethodPut::~MethodPut() {}

void			MethodPut::setUploadPath()
{
	std::string uploadUri(data.location->uploadStore);
	std::size_t found = data.uri.request_uri.find_last_of('/');
	std::string fname =	data.uri.request_uri.substr(found, data.uri.request_uri.length() - 1);
	uploadUri += fname;
	data.uri.script_name = uploadUri;

	if (fileExists(data.uri.script_name.c_str()))
		_statusCode = 0;
}

MethodStatus	MethodPut::prepareFdToWrite()
{
	if (_fd == -1)
		_fd = open(data.uri.script_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
	else
		_fd = open(data.uri.script_name.c_str(), O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0644);

	if (_fd < 0){
		_statusCode = 404;
		return error;
	}
	return ok;
}

MethodStatus	MethodPut::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	if (_statusCode == 403 || _statusCode == 405)
		return ok;

	if (data.location->uploadPass && _fd == -1)
		setUploadPath();

	if (prepareFdToWrite() == error)
		return error;

	ssize_t res = write(_fd, requestBody.c_str(), requestBody.length());
	close(_fd);
	if (res < 0 || static_cast<size_t>(res) < requestBody.length()){
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
