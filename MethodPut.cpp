#include "MethodPut.hpp"

MethodPut::~MethodPut() {};

MethodStatus	MethodPut::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	_fd = open(data.uri.script_name.c_str(), O_WRONLY | O_APPEND | O_CREAT| O_NONBLOCK, 0644);
	if (_fd < 0){
		_statusCode = notFound;
		return error;//
	}

	int res = write(_fd, requestBody.c_str(), requestBody.length());
	close(_fd);
	if (res < requestBody.length()){
		_statusCode = notFound;//internalError;
		return error;//
	}

	_statusCode = created;//201

	if (bodyStatus == inprogress)
		return inprogress;// receive more body in the next iteration
	return ok;
};

MethodStatus	MethodPut::manageRequest()
{
	if (!data.location->putAvailable)
		_statusCode = methodNotAllowed;

	else if (_bodyType == bodyIsAutoindex)
	{
		_statusCode = 403;
		// _bodyType = bodyNotDefined;///// bodyIsError
	}
	return ok;
};

MethodStatus	MethodPut::createHeader()
{
	if (_bodyType == bodyIsCGI){
		// _statusCode = cgi.init(data);//always returns 200
		return ok;
	}
	// if (_bodyType == bodyIsTextErrorPage) // в путе всегда bodyIsEmpty или bodyIsAutoindex
	// 	generateErrorPage(_body);

	Header		header(data.uri.script_name, data.location->root, _statusCode);
	stringMap	hmap;
	std::cout << "////\t\t PUT METHOD, statusCode: " << _statusCode << std::endl;

	header.createGeneralHeaders(hmap);
	// header.addContentLengthHeader(hmap, _body);// так мы не отправим стр с ошибкой
	hmap.insert(std::pair<std::string, std::string>("Content-Length", "0"));//can it be specified in request before?

	if (_statusCode == 405)
		header.addAllowHeader(hmap, *data.location);

	std::string headerStr;
	header.headersToString(hmap, headerStr);
	_body.insert(0, headerStr);

	return ok;
};

MethodStatus	MethodPut::sendHeader(int socket){ return ok; };

MethodStatus	MethodPut::sendResponse(int socket){ return ok; };
