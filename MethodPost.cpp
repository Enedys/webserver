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
	MethodStatus status;
	std::string str;
	status = cgi.output(str);
	send(socket, str.c_str(), str.length(), MSG_DONTWAIT); // TODO buffer!!
	if (status == ok)
		send(socket, "\r\n\r\n", 4, MSG_DONTWAIT);
	return (status);
};

MethodStatus	MethodPost::sendHeader(int socket)
{
	std::string str;
	while (!cgi.isHeadersDone())
		cgi.output(str);
	// output str to socket, probably not;
	std::string st = "HTTP/1.1 200 OK\r\n";
	send(socket, st.c_str(), st.length(), MSG_DONTWAIT);
	for (auto it = _headersMap.begin(); it != _headersMap.end(); ++it)
	{
		send(socket, it->first.c_str(), it->first.length(), MSG_DONTWAIT);
		send(socket, ": ", 2, MSG_DONTWAIT);
		send(socket, it->second.c_str(), it->second.length(), MSG_DONTWAIT);
		send(socket, "\r\n", 1, MSG_DONTWAIT);
	}
	return (ok);
};
