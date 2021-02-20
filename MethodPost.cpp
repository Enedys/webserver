#include "MethodPost.hpp"

MethodPost::MethodPost(t_serv const &config, int &code, stringMap const &headers) : AMethod(config, code, headers) {};
MethodPost::~MethodPost() {};
MethodStatus	MethodPost::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodPost::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodPost::createHeader(std::string const &_path) { return (ok); };
MethodStatus	MethodPost::sendBody(int socket) { return (ok); };
MethodStatus	MethodPost::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
