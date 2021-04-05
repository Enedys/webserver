#include "MethodPost.hpp"
// manage request, process_body, create_header, send header, send body
MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::manageRequest()
{
//	constMapIter cgi_iter = data.location->cgi.find(data.uri.extension);
//	if (cgi_iter ==  data.location->cgi.cend())
//	{
//		_statusCode = 405;
//		return (error);
//	}
//	data.setCGIbin((*cgi_iter).second);
//	data.createCGIEnv();
//	if (!data.cgi_conf)
//		return (error);
//	_statusCode = cgi.init(data);
//	if (_statusCode < 200 || _statusCode > 206)
//		return error;
//	return ok;
	if (_bodyType == bodyIsAutoindex && (generateIdxPage(_body) < 0))
	{
		_statusCode = 404;// дальше показать индексовую страницу, если она есть
		// _bodyType = bodyNotDefined;
	}
	if (_bodyType == bodyIsCGI){
		constMapIter cgi_iter = data.location->cgi.find(data.uri.extension);
		if (cgi_iter ==  data.location->cgi.cend())
			return (error);
		data.setCGIbin((*cgi_iter).second);
		// data.cgi_bin = (*cgi_iter).second;
		// if (!fileExist(_data.cgi_bin))
		// 	return (error);
		data.createCGIEnv();
		if (!data.cgi_conf)
			return (error);
		_statusCode = cgi.init(data);//return 200
		return ok;
	}
	return ok;//
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
	std::cout << _statusCode;
	if (_statusCode < 200 || _statusCode > 206)
	{
		std::string str;
		_header = new Header(data.uri.script_name, data.location->root, _statusCode);
		// _header->generateErrorPage(str, data.serv->error_pages);
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

MethodStatus	MethodPost::sendResponse(int socket)
{
//	MethodStatus debug;
//	if (_statusCode == 200)
//	{
//		debug = cgi.superSmartOutput(socket);
//		if (debug == inprogress)
//			std::cerr << "IN _ P R O G R E S S \n";
//		return debug;
//	}
//	else
//		return sendError(socket);
	return AMethod::sendResponse(socket);

};


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
