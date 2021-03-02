#include "MethodPost.hpp"

MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::createHeader()
{
	// first of all I need to know is it CGI. todo: care about it later
	char **args;
	args = (char **)malloc(sizeof(char *) * 3); // todo: free
	args[2] = 0;


	std::string ext = data.uri.script_name.substr(data.uri.script_name.find_last_of('.') + 1, data.uri.script_name.size());
	std::string bin = data.location->cgi.find(ext)->second;
	args[0] = (char *)bin.c_str();
	args[1] = (char *)data.uri.script_name.c_str();
	cgi.setEnv(data.cgi_conf);
	cgi.setEnv(NULL);
	cgi.setExecpath((char *)bin.c_str());
	cgi.setArgs(args);
	cgi.init();
	std::cout << "create header!\n";
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
	std::cout << "SENDING BODYU \n";
	status = cgi.output(str);
	if (!str.empty())
	{
		std::cout << "I'VE GOT THAT!\n";
		std::cout << str;
		std::string num = size2Hex(str.length());
		num = num + "\r\n";
		send(socket, num.c_str(), num.length(), MSG_DONTWAIT);
		send(socket, str.c_str(), str.length(), MSG_DONTWAIT); // TODO buffer!!
		send(socket, "\r\n", 2, MSG_DONTWAIT);
		str.clear();
	//	send(socket, "0\r\n\r\n", 5, MSG_DONTWAIT);
	}
	if (status == ok)
		send(socket, "0\r\n\r\n", 5, MSG_DONTWAIT);
	return (status);
};

MethodStatus	MethodPost::sendHeader(int socket)
{
	std::cout << "send header!\n";
	std::string str;
	MethodStatus status = ok;
	while (!cgi.isHeadersDone() && status != error)
		status = cgi.output(str);
	if (status == error)
		return (error);
	// output str to socket, probably not;
	std::string st = "HTTP/1.1 200 OK\r\n";
	send(socket, st.c_str(), st.length(), MSG_DONTWAIT);
//	st = "Content-type: text/html; charset=UTF-8\r\n";
	if (!cgi.isHeadersNotFound())
	{
		for (auto it = cgi._headersMap.begin(); it != cgi._headersMap.end(); ++it)
		{
			send(socket, it->first.c_str(), it->first.length(), MSG_DONTWAIT);
			send(socket, ": ", 2, MSG_DONTWAIT);
			send(socket, it->second.c_str(), it->second.length(), MSG_DONTWAIT); // todo: \r\n is included.
			//send(socket, "\r\n", 2, MSG_DONTWAIT);
		}
	}
	st = "Transfer-Encoding: chunked\r\n\r\n";
	send(socket, st.c_str(), st.length(), MSG_DONTWAIT);
	return (ok);
};
