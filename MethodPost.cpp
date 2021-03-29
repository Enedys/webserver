#include "MethodPost.hpp"
// manage request, process_body, create_header, send header, send body
MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::manageRequest()
{
	char **args;
	args = (char **)malloc(sizeof(char *) * 3); // todo: free
	std::string ext = data.uri.script_name.substr(data.uri.script_name.find_last_of('.') + 1, data.uri.script_name.size());
	ext = data.uri.extension;
	std::string bin = data.location->cgi.find(ext)->second; // todo: not found cgi path;
	args[0] = (char *)bin.c_str();
	args[1] = (char *)data.uri.script_name.c_str(); // todo: check, no script found
	args[2] = 0;
	_statusCode = 200;
	if (bin.empty())
	{
		_statusCode = 405;
		return(error); // if error jumps to create header;
	}
	cgi.setEnv(data.cgi_conf);
	cgi.setEnv(NULL);
	cgi.setExecpath((char *)bin.c_str());
	cgi.setArgs(args);
	cgi.init();//checks
	std::cout << "create header!\n";
	return (ok);
};

MethodStatus MethodPost::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	// todo: bodystartus - ok, end of input;
	cgi.input(requestBody, bodyStatus);
	//	todo: update _statusCode according to cgi output status
	return (bodyStatus);
};

MethodStatus	MethodPost::createHeader()
{
	if (_statusCode < 200 || _statusCode > 206)
	{
		_header->generateErrorPage(_body, data.serv->error_pages);
		_header = new Header(data.uri.script_name, data.location->root, _statusCode);
		_header->createGeneralHeaders(_headersMap);
	}
//		if (_statusCode == 405)
//			_header->addAllowHeader(_headersMap, *data.serv);
	return(ok);
}

MethodStatus	MethodPost::sendHeader(int socket)
{
	return ok;
};

MethodStatus	MethodPost::sendBody(int socket)
{
	MethodStatus debug;
	if (_statusCode == 200)
	{
		debug = cgi.superSmartOutput(socket);
		return debug;
	}
	else
		return sendError(socket);

};



MethodStatus		MethodPost::sendResponse(int socket) {
	return ok;
}

MethodStatus MethodPost::sendError(int socket)
{
	int r;
	r = send(socket, _body.c_str(), BUFSIZ, MSG_DONTWAIT);
	if (r < _body.length())
	{
		_body.substr(r, _body.length());
		return inprogress;
	}
	return ok;
}
