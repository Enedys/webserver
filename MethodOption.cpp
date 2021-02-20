#include "MethodOption.hpp"

MethodOption::~MethodOption() {};
MethodStatus	MethodOption::createHeader() { return (ok); };
MethodStatus	MethodOption::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodOption::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodOption::sendBody(int socket) { return (ok); };
MethodStatus	MethodOption::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
