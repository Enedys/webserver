#include "MethodDelete.hpp"

MethodDelete::~MethodDelete() {};
MethodStatus	MethodDelete::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return ok; };
MethodStatus	MethodDelete::sendResponse(int socket) { return ok; }

int				MethodDelete::deleteDirectory(std::string const &path)
{
	if (rmdir(path.c_str()) == 0)
		return okSuccess;

	int status = 200;
	DIR *dir = opendir(path.c_str());
	if (!dir)
		return errorOpeningURL;

	char cwd[256];
	std::string curDir = getcwd(cwd, sizeof(cwd));
	chdir(path.c_str());

	struct dirent	*cur;
	errno = 0;
	while ((cur = readdir(dir)) != NULL){
		if (cur->d_name[0] == '.')
			continue ;
		struct stat	st;
		stat(cur->d_name, &st);
		if (S_ISDIR(st.st_mode))
			status = deleteDirectory(cur->d_name);
		else if (open(cur->d_name, 0) < 0 || unlink(cur->d_name) == -1)
			status = ImaTeapot;//-1;//404//500
	}
	closedir(dir);
	chdir(curDir.c_str());
	if (status == 200)
		rmdir(path.c_str());
	return status;
}

MethodStatus	MethodDelete::manageRequest()
{
	if (!data.location->deleteAvailable
	|| data.uri.script_name.length() >= PATH_MAX - 1)//
	{
		_statusCode = methodNotAllowed;
		return error;
	}
	_statusCode = okSuccess;

	struct stat	st;
	if (stat(data.uri.script_name.c_str(), &st) == -1){
		_statusCode = notFound;
		return ok;
	}
	if (S_ISDIR(st.st_mode))
		_statusCode = deleteDirectory(data.uri.script_name);
	else if (open(data.uri.script_name.c_str(), 0) < 0 || unlink(data.uri.script_name.c_str()) == -1)//EACCES//EBUSY//EISDIR//
		_statusCode = errorOpeningURL;

	return ok;
};

MethodStatus	MethodDelete::createHeader()
{
	// _header = new Header(data.uri.script_name, _statusCode);
	_header = new Header(data.uri.script_name, data.location->root, _statusCode);
	std::cout << "////\t\t DELETE METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap);
	// if (_statusCode < 200 || _statusCode > 206)
	// 	_header->generateErrorPage(_body, data.serv->error_pages);
	_headersMap.insert(std::pair<std::string, std::string>("Content-Length", "0"));//
	if (_statusCode == methodNotAllowed)
		_header->addAllowHeader(_headersMap, *data.location);
	return ok;
};

MethodStatus	MethodDelete::sendHeader(int socket){ return ok; };

MethodStatus	MethodDelete::sendBody(int socket)
{
	std::string	response;
	size_t		sentBytes;

	if (_remainder.length())
		response = _remainder;
	if (_statusCode != okSendingInProgress)
		_header->headersToString(_headersMap, response);

	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || errno == EMSGSIZE){
		_statusCode = errorSendingResponse;
		return error;
	}
	if (sentBytes < response.length()){
		_remainder.assign(response.c_str(), sentBytes, response.length() - sentBytes);
		_statusCode = okSendingInProgress;
		return inprogress;
	};
	_remainder.clear();
	return ok;
};
