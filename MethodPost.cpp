#include "MethodPost.hpp"

MethodPost::~MethodPost() {};
MethodStatus	MethodPost::createHeader() { return (ok); };
MethodStatus	MethodPost::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodPost::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodPost::sendBody(int socket) { return (ok); };
MethodStatus	MethodPost::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
