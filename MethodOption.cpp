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

	_statusCode = _statusCode == 0 ? 204 : _statusCode;

	return ok;
};
