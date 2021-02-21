#include "MethodPut.hpp"

MethodPut::~MethodPut() {};
MethodStatus	MethodPut::createHeader(std::string const &_path)
{
	return (ok);
};

MethodStatus	MethodPut::processBody(const std::string &requestBody)
{
	int size;
	if (!_inProgress)
	{
		_uri.pathFd = open(_uri.path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
		if (_uri.pathFd < 0)
			return (error); // TODO: send error or something
		_inProgress = true;
	}
	size = write(_uri.pathFd, requestBody.c_str(), requestBody.length());
	if (size < 0)
		return (error); // TODO: can't write to file, send error
	return (ok); // TODO: or in progress?
};

MethodStatus	MethodPut::manageRequest(std::string const &path)
{
	return (ok);
};

MethodStatus	MethodPut::sendBody(int socket)
{
	return (ok);
};

MethodStatus	MethodPut::sendHeader(int socket)
{
	_inProgress = false;
	close(_uri.pathFd);
	return (AMethod::sendHeader(socket));
};
