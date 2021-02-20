#include "MethodPut.hpp"

MethodPut::~MethodPut() {};
MethodStatus	MethodPut::createHeader(std::string const &_path) { return (ok); };
MethodStatus	MethodPut::processBody(const std::string &requestBody) { return (ok); };
MethodStatus	MethodPut::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodPut::sendBody(int socket) { return (ok); };
MethodStatus	MethodPut::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
