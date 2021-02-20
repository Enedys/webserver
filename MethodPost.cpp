#include "MethodPost.hpp"

MethodPost::MethodPost(MethodPost const & src) {};
MethodPost & MethodPost::operator=(MethodPost const & src) {};
MethodPost::MethodPost() {};

MethodPost::MethodPost(t_serv const &config, int &code, stringMap const &headersMapRequest) {};
MethodPost::~MethodPost() {};

MethodStatus	MethodPost::readRequestBody() { return (ok); };
MethodStatus	MethodPost::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodPost::createHeader() { return (ok); };
MethodStatus	MethodPost::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodPost::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodPost::sendBody(int socket) { return (ok); };
MethodStatus	MethodPost::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
