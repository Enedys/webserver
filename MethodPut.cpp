#include "MethodPut.hpp"

MethodPut::~MethodPut() {};
MethodStatus	MethodPut::createHeader() { return (ok); };
MethodStatus	MethodPut::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodPut::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodPut::sendBody(int socket) { return (ok); };
MethodStatus	MethodPut::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
