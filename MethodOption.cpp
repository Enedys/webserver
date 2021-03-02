#include "MethodOption.hpp"

// MethodOption::MethodOption(t_serv const &config, int &code, stringMap const &headers) : AMethod(config, code, headers) {};
MethodOption::~MethodOption() {};
MethodStatus	MethodOption::createHeader() { return (ok); };
MethodStatus	MethodOption::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodOption::manageRequest() { return (ok); };
MethodStatus	MethodOption::sendBody(int socket) { return (ok); };
MethodStatus	MethodOption::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};


MethodStatus		MethodOption::sendResponse(int socket) {
	return ok;
}