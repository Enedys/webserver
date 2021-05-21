#include "MethodPut.hpp"

MethodPut::~MethodPut() {}

void			MethodPut::setUploadPath()
{
	std::string uploadUri(data.location->uploadStore);
	std::size_t found = data.uri.request_uri.find_last_of('/');
	std::string fname =	data.uri.request_uri.substr(found, data.uri.request_uri.length() - 1);
	uploadUri += fname;
	data.uri.script_name = uploadUri;

	//* checking in the updated folder
	if (fileExists(data.uri.script_name.c_str()))
		_statusCode = 0;
}

MethodStatus	MethodPut::prepareFdToWrite()
{
	if (_fd == -1)
		_fd = open(data.uri.script_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
	if (_fd < 0){
		_statusCode = 404;
		return error;
	}
	return ok;
}

MethodStatus	MethodPut::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	//* if error occured earlier (but file may not exist before)
	if (_statusCode >= 400 && _statusCode != 404)
		return ok;

	//* here for the first time, setting up the folder to load into if it was set
	if (data.location->uploadPass && _fd == -1)
		setUploadPath();

	if (prepareFdToWrite() == error)
		return error;

	ssize_t res = write(_fd, requestBody.c_str(), requestBody.length());

	if (res < 0 || static_cast<size_t>(res) < requestBody.length()){
		_statusCode = 404;
		return error;
	}

	_statusCode = _statusCode == 404 ? 201 : 204;

	if (bodyStatus == inprogress)
		return inprogress;
	close(_fd);
	_fd = -1;
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
