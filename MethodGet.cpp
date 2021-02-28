#include "MethodGet.hpp"

MethodGet::~MethodGet() {};
MethodStatus	MethodGet::createHeader() { return (ok); };
MethodStatus	MethodGet::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodGet::manageRequest() { return (ok); };
MethodStatus	MethodGet::sendBody(int socket) { return (ok); };
MethodStatus	MethodGet::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
}