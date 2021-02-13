#include "MethodOption.hpp"

MethodOption::~MethodOption() {};
MethodStatus	MethodOption::createHeader() { return (ok); };
MethodStatus	MethodOption::createErrorHeader() { return (ok); };
MethodStatus	MethodOption::sendHeader(int socket) { return (ok); };
MethodStatus	MethodOption::readBody(int socket) { return (ok); };
MethodStatus	MethodOption::processRequest(std::string const &location) { return (ok); };
MethodStatus	MethodOption::sendBody(int socket) { return (ok); };
