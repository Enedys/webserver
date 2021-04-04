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

MethodStatus	MethodDelete::manageRequest()//do i need to check if statuscode is already error?
{
	if (!data.location->deleteAvailable){
		_statusCode = methodNotAllowed;
		return error;//
	}
	// if (_bodyType == bodyIsAutoindex) // == нельзя удалить папку
	// 	_statusCode = errorOpeningURL;

	struct stat	st;
	stat(data.uri.script_name.c_str(), &st);
	if (S_ISDIR(st.st_mode))
		_statusCode = deleteDirectory(data.uri.script_name);
	// else if (open(data.uri.script_name.c_str(), 0) < 0 || unlink(data.uri.script_name.c_str()) == -1)//EACCES//EBUSY//EISDIR//
	else if (unlink(data.uri.script_name.c_str()) == -1)
		_statusCode = errorOpeningURL;
	else
		_statusCode = okSuccess;

	return ok;
};

MethodStatus	MethodDelete::createHeader()
{
	if (_bodyType == bodyIsTextErrorPage)
		generateErrorPage(_body);

	Header		header(data.uri.script_name, data.location->root, _statusCode);
	stringMap	hmap;

	std::cout << "////\t\t DELETE METHOD, statusCode: " << _statusCode << std::endl;

	header.createGeneralHeaders(hmap);
	// header.addContentLengthHeader(hmap, _body);
	hmap.insert(std::pair<std::string, std::string>("Content-Length", "0"));//can it be specified in request before?

	if (_statusCode == 405)
		header.addAllowHeader(hmap, *data.location);

	std::string headerStr;
	header.headersToString(hmap, headerStr);
	_body.insert(0, headerStr);

	return ok;
};

MethodStatus	MethodDelete::sendHeader(int socket){ return ok; };
