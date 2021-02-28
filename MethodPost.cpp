#include "MethodPost.hpp"

MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::createHeader(std::string const &_path)
{
	// first of all I need to know is it CGI. todo: care about it later
	char **args;
	args = (char **)malloc(sizeof(char *) * 3);
	args[2] = 0;
	args[1] = (char *)reqData.location->root.c_str();
	args[0] = (char *)reqData.pathToFile.c_str();

	cgi.setEnv(reqData.cgi_conf);
	cgi.setExecpath(reqData.pathToFile.c_str());
	cgi.setArgs(args);
	cgi.init();
	return (ok);
};

MethodStatus MethodPost::processBody(const std::string &requestBody)
{
	cgi.input(requestBody);
	return (ok);
};

MethodStatus	MethodPost::manageRequest(std::string const &path)
{
	return (ok);
};

MethodStatus	MethodPost::sendBody(int socket)
{
	return (ok);
};

MethodStatus	MethodPost::sendHeader(int socket)
{

	return (AMethod::sendHeader(socket));
};
