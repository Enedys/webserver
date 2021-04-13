#include "MethodDelete.hpp"

MethodDelete::~MethodDelete() {}
MethodStatus	MethodDelete::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	(void)requestBody;
	(void)bodyStatus;
	return ok;
}

int				MethodDelete::deleteDirectory(std::string const &path)
{
	if (rmdir(path.c_str()) == 0)
		return 200;

	int status = 200;
	DIR *dir = opendir(path.c_str());
	if (!dir)
		return 403;

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
			status = 418;
	}
	closedir(dir);
	chdir(curDir.c_str());
	if (status == 200)
		rmdir(path.c_str());
	return status;
}

MethodStatus	MethodDelete::manageRequest()
{
	// if (_bodyType == bodyIsAutoindex) // to disable folder deleting
	// 	_statusCode = 403;

	_bodyType = bodyIsEmpty;

	if (!data.location->deleteAvailable)
		_statusCode = 405;
	if (_statusCode != 0)
		return error;

	struct stat	st;
	stat(data.uri.script_name.c_str(), &st);
	if (S_ISDIR(st.st_mode))
		_statusCode = deleteDirectory(data.uri.script_name);
	else if (unlink(data.uri.script_name.c_str()) == -1)
		_statusCode = 403;
	else
		_statusCode = 200;

	return ok;
}
