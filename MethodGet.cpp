#include "MethodGet.hpp"

MethodGet::~MethodGet() {};
MethodStatus	MethodGet::createHeader() { return (ok); };
MethodStatus	MethodGet::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodGet::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodGet::sendBody(int socket) { return (ok); };
MethodStatus	MethodGet::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
}