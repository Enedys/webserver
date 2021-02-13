#include "MethodError.hpp"

MethodStatus	MethodError::createHeader() { return (ok); };
MethodStatus	MethodError::createErrorHeader() { return (ok); };
MethodStatus	MethodError::sendHeader(int socket) { return (ok); };
MethodStatus	MethodError::readBody(int socket) { return (ok); };
MethodStatus	MethodError::processRequest(std::string	const &location) { return (ok); };
MethodStatus	MethodError::sendBody(int socket) { return (ok); };

MethodError::~MethodError()
{
}