#include "MethodHead.hpp"
#include "Header.hpp"

MethodHead::~MethodHead() {}

MethodStatus 	MethodHead::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	(void)requestBody;
	(void)bodyStatus;
	return ok;
}

MethodStatus	MethodHead::manageRequest()
{
	std::cout << "_bodyType manageRequest: " << _bodyType << std::endl;

	if (!data.location->headAvailable)
		_statusCode = 405;

	else if (_bodyType == bodyIsAutoindex)
		_statusCode = 403;

	_bodyType = bodyIsEmpty;

	return ok;
}
