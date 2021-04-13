#include "MethodOption.hpp"

MethodOption::~MethodOption() {}
MethodStatus	MethodOption::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	(void)requestBody;
	(void)bodyStatus;
	return ok;
}

MethodStatus	MethodOption::manageRequest()
{
	_bodyType = bodyIsEmpty;

	if (!data.location->optionsAvailable)
		_statusCode = 405;

	else if (_bodyType == bodyIsAutoindex)
		_statusCode = 403;

	_statusCode = _statusCode == 0 ? 204 : _statusCode;

	return ok;
}
