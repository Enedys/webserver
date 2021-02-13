#include "MethodHead.hpp"

MethodHead::~MethodHead() {};
MethodStatus	MethodHead::createHeader() { return (ok); };
MethodStatus	MethodHead::createErrorHeader() { return (ok); };
MethodStatus	MethodHead::sendHeader(int socket) { return (ok); };
MethodStatus	MethodHead::readBody(int socket) { return (ok); };
MethodStatus	MethodHead::processRequest(std::string	const &location) { return (ok); };
MethodStatus	MethodHead::sendBody(int socket) { return (ok); };
