#include "MethodPost.hpp"
// manage request, process_body, create_header, send header, send body
MethodPost::~MethodPost()
{

}

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
	if (!data.location->postAvailable)
	{
		_statusCode = 405;
		_bodyType = bodyIsTextErrorPage;
		return error;
	}
	if (_bodyType == bodyIsAutoindex)
	{
		_statusCode = 403; // todo: !! ??
		_bodyType = bodyIsTextErrorPage;
		return error;
	}
	if (data.location->cgi.find(data.uri.extension) == data.location->cgi.end()) // todo: cend is c++11
	{
		_statusCode = 201;
		_bodyType = bodyIsCGI;
		return ok;
	}
//	if (_bodyType == bodyNotDefined)
//	{
//		_statusCode = 200; // todo: is for tester;
//		_bodyType = bodyIsCGI;
//		return ok;
//	}
	if (_bodyType == bodyNotDefined)
	{

	}
	if (_bodyType == bodyIsCGI || _bodyType == bodyNotDefined) {
		_statusCode = 200; // todo: is for tester;
		_bodyType = bodyIsCGI;
		constMapIter cgi_iter = data.location->cgi.find(data.uri.extension);
		if (cgi_iter ==  data.location->cgi.end()) // todo: cend is c++11
		{
			_statusCode = 405;
			return (error);
		}
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
}


MethodStatus MethodPost::processFile(const std::string &requestBody, MethodStatus bodyStatus)
{
	if (_fd == -1)
	{
		timeval tv;
		gettimeofday(&tv, NULL);
		std::string tmp = "/" + size2Dec(tv.tv_sec * 1000000 + tv.tv_usec);
		data.uri.script_name = data.location->root + tmp;
		data.uri.request_uri = data.location->path + tmp;
		_fd = open(data.uri.script_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
	}
	size_t res = write(_fd, requestBody.c_str(), requestBody.length());
	if (res < requestBody.length()){
		_statusCode = 404;
		return error;
	}

	return bodyStatus;
}

MethodStatus MethodPost::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	if (_statusCode == 201)
		return processFile(requestBody, bodyStatus);
	if (_statusCode >= 400)
		return bodyStatus;
	// todo: bodystartus - ok, end of input;
	cgi.input(requestBody, bodyStatus);
	std::cout << "\n\nbody\n\n";

	//	todo: update _statusCode according to cgi output status
	return (bodyStatus);
}

MethodStatus	MethodPost::createHeader()
{
 	if (_statusCode == 413 && _fd != -1)
	{
		close(_fd);
	 	_fd = -1;
	 	unlink(data.uri.script_name.c_str());
	}
	return AMethod::createHeader();
}
// {
// 	std::cout << "_bodyType createHeader: " << _bodyType << std::endl;
// 	if (_bodyType == bodyIsCGI){
// 		// _statusCode = cgi.init(data);//return 200
// 		return ok;
// 	}

// 	if (_bodyType == bodyIsTextErrorPage)//bodyNotDefined)//)////->сказать Дане, надо bodyIsTextErrorPage)
// 		generateErrorPage(_body);

// 	Header		header(data.uri.script_name, data.location->root, _statusCode);
// 	stringMap	hmap;
// 	std::cout << "\n////\tPOST METHOD, statusCode: " << _statusCode << std::endl;

// 	header.createGeneralHeaders(hmap);
// 	header.addContentLengthHeader(hmap, _body);//for GET//body for auto+error//if not dir!

// 	// if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206))
// 	// 	header.createEntityHeaders(hmap);
// 	if (_statusCode == 405)
// 		header.addAllowHeader(hmap);
// 	// header.addLocationHeader(hmap);
// 	// header.addRetryAfterHeader(hmap);//503 429
// 	// header.addTransferEncodingHeader(hmap, hmapRequest);
// 	// header.addAuthenticateHeader(hmap);

// 	std::string headerStr;
// 	header.headersToString(hmap, headerStr);
// 	_body.insert(0, headerStr);

// 	return ok;
// }

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

}


MethodStatus MethodPost::sendError(int socket)
{
	int r;
	r = send(socket, _body.c_str(), BUFSIZ, MSG_DONTWAIT);
	if (r < static_cast <int> (_body.length()))
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

