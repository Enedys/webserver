#include "MethodHead.hpp"
#include "Header.hpp"

MethodHead::~MethodHead() {};
MethodStatus 	MethodHead::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodHead::sendHeader(int socket) { return (ok); };

MethodStatus	MethodHead::manageRequest()
{
	std::cout << "_bodyType manageRequest: " << _bodyType << std::endl;

	if (!data.location->headAvailable)
		_statusCode = methodNotAllowed;

	else if (_bodyType == bodyIsAutoindex)//isCGI
		_statusCode = 403;

	_bodyType = bodyIsEmpty;

	return ok;
};
