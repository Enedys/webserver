#include "MethodGet.hpp"

MethodGet::~MethodGet() {};
MethodStatus	MethodGet::createHeader(std::string const &_path) { return (ok); };
MethodStatus	MethodGet::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodGet::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodGet::sendBody(int socket) { return (ok); };
MethodStatus	MethodGet::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
}