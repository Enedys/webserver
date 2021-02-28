#include "MethodPut.hpp"

MethodPut::~MethodPut() {};
MethodStatus	MethodPut::createHeader() { return (ok); };
MethodStatus	MethodPut::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodPut::manageRequest() { return (ok); };
MethodStatus	MethodPut::sendBody(int socket) { return (ok); };
MethodStatus	MethodPut::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
