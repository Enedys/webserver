#include "MethodPut.hpp"

MethodPut::~MethodPut() {};
MethodStatus	MethodPut::createHeader() { return (ok); };
MethodStatus	MethodPut::createErrorHeader() { return (ok); };
MethodStatus	MethodPut::sendHeader(int socket) { return (ok); };
MethodStatus	MethodPut::readBody(int socket) { return (ok); };
MethodStatus	MethodPut::processRequest(std::string	const &location) { return (ok); };
MethodStatus	MethodPut::sendBody(int socket) { return (ok); };
