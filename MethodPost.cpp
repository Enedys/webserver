#include "MethodPost.hpp"

MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::createHeader()
{
	// first of all I need to know is it CGI. todo: care about it later
	char **args;
	args = (char **)malloc(sizeof(char *) * 3);
	args[2] = 0;
	args[1] = (char *)data.location->root.c_str();
	args[0] = (char *)data.pathToFile.c_str();

	cgi.setEnv(data.cgi_conf);
	cgi.setExecpath(data.pathToFile.c_str());
	cgi.setArgs(args);
	cgi.init();
	return (ok);
};

MethodStatus MethodPost::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	cgi.input(requestBody);
	return (ok);
};

MethodStatus	MethodPost::manageRequest()
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
	for (auto it = cgi._headersMap.begin(); it != cgi._headersMap.end(); ++it)
	{
		send(socket, it->first.c_str(), it->first.length(), MSG_DONTWAIT);
		send(socket, ": ", 2, MSG_DONTWAIT);
		send(socket, it->second.c_str(), it->second.length(), MSG_DONTWAIT);
		send(socket, "\r\n", 1, MSG_DONTWAIT);
	}
	return (ok);
};
