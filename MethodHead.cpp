#include "MethodHead.hpp"

MethodHead::~MethodHead() {};
MethodStatus	MethodHead::createHeader(std::string const &_path) { return (ok); };
MethodStatus 	MethodHead::processBody(const std::string &requestBody) { return (ok); };
MethodStatus	MethodHead::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodHead::sendBody(int socket) { return (ok); };
MethodStatus	MethodHead::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
