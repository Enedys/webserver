#include "MethodHead.hpp"

MethodHead::~MethodHead() {};
MethodStatus	MethodHead::createHeader() { return (ok); };
MethodStatus 	MethodHead::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodHead::manageRequest() { return (ok); };
MethodStatus	MethodHead::sendBody(int socket) { return (ok); };
MethodStatus	MethodHead::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
