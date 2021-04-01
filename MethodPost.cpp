#include "MethodPost.hpp"
// manage request, process_body, create_header, send header, send body
MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::manageRequest()
{
//	char **args;
//	args = (char **)malloc(sizeof(char *) * 3); // todo: free
//	std::string ext = data.uri.script_name.substr(data.uri.script_name.find_last_of('.') + 1, data.uri.script_name.size());
//	ext = data.uri.extension;
//	auto it = data.location->cgi.find(ext); // todo !!
//	//std::map<std::string, std::string> kekw;
//	std::string bin;
//	if (it != data.location->cgi.end())
//		bin = it->second; // todo: not found cgi path;
//	_statusCode = 200;
//	if (bin.empty())
//	{
//		_statusCode = 405;
//		return (error); // if error jumps to create header;
//	}
//	if (!fileExists((char *)data.uri.script_name.c_str()))
//	{
//		_statusCode = 404;
//		return (error);
//	}
//	args[0] = (char *)bin.c_str();
//	args[1] = (char *)data.uri.script_name.c_str(); // todo: check, no script found
//	args[2] = 0;
//	cgi.setEnv(data.cgi_conf);
//	int i = 0;
//	std::cout << "\n------\n";
//	while (*(data.cgi_conf + i))
//		std::cout << *(data.cgi_conf + i++)<< std::endl;
//	std::cout << "\n------\n";
////	sleep(600);
////	cgi.setEnv(NULL);
//	cgi.setExecpath((char *)bin.c_str());
//	cgi.setArgs(args);
//	cgi.init();//checks
//	std::cout << "\n\ncreate header!!\n\n";
//	return (ok);
	_statusCode = cgi.init(data);
	if (_statusCode < 200 || _statusCode > 206)
		return error;
	return ok;
};

MethodStatus MethodPost::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	// todo: bodystartus - ok, end of input;
	cgi.input(requestBody, bodyStatus);
	std::cout << "\n\nbody\n\n";

	//	todo: update _statusCode according to cgi output status
	return (bodyStatus);
};

MethodStatus	MethodPost::createHeader()
{
//	std::cerr << "\n\nCREATE HEADER!!!!!!!\n\n";
	if (_statusCode < 200 || _statusCode > 206)
	{
		std::string str;
		_header = new Header(data.uri.script_name, data.location->root, _statusCode);
		_header->generateErrorPage(str, data.serv->error_pages);
		_header->createGeneralHeaders(_headersMap);
		_header->addContentLengthHeader(_headersMap, str);
		_header->headersToString(_headersMap, _body);
		_body = _body + str;
		delete _header;
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
		if (debug == inprogress)
			std::cerr << "IN _ P R O G R E S S \n";
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

bool MethodPost::fileExists(char *filename)
{
	struct stat   buffer;
	return (stat(filename, &buffer) == 0);
}
