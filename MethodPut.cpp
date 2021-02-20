#include "MethodPut.hpp"

MethodPut::MethodPut(t_serv const &config, int &code, stringMap const &headers) : AMethod(config, code, headers) {};
MethodPut::~MethodPut() {};

MethodStatus	MethodPut::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodPut::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodPut::createHeader(std::string const &_path) { return (ok); };
MethodStatus	MethodPut::sendBody(int socket) { return (ok); };
MethodStatus	MethodPut::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
