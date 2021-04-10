#include "MethodOption.hpp"

MethodOption::~MethodOption() {};
MethodStatus	MethodOption::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return ok; };
MethodStatus	MethodOption::sendHeader(int socket){ return ok; };

MethodStatus	MethodOption::manageRequest()
{
	_bodyType = bodyIsEmpty;

	if (!data.location->optionsAvailable)
		_statusCode = methodNotAllowed;

	else if (_bodyType == bodyIsAutoindex)
		_statusCode = 403;

	return ok;
};

// MethodStatus	MethodOption::createHeader()
// {
// 	Header		header(data.uri.script_name, data.location->root, _statusCode);
// 	stringMap	hmap;
// 	std::cout << "////\t\t OPTIONS METHOD, statusCode: " << _statusCode << std::endl;

// 	header.createGeneralHeaders(hmap);
// 	hmap.insert(std::pair<std::string, std::string>("Content-Length", "0"));//can it be specified in request before?

// 	header.addAllowHeader(hmap, *data.location);

// 	std::string headerStr;
// 	header.headersToString(hmap, headerStr);
// 	_body.insert(0, headerStr);

// 	return ok;
// };
