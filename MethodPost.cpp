#include "MethodPost.hpp"

MethodPost::~MethodPost() {};
MethodStatus	MethodPost::createHeader() { return (ok); };
MethodStatus	MethodPost::createErrorHeader() { return (ok); };
MethodStatus	MethodPost::sendHeader(int socket) { return (ok); };
MethodStatus	MethodPost::readBody(int socket) { return (ok); };
MethodStatus	MethodPost::processRequest(std::string	const &location) { return (ok); };
MethodStatus	MethodPost::sendBody(int socket) { return (ok); };
