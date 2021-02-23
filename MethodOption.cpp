#include "MethodOption.hpp"

MethodOption::MethodOption(t_serv const &config, int &code, stringMap const &headers) : AMethod(config, code, headers) {};
MethodOption::~MethodOption() {};
MethodStatus	MethodOption::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodOption::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodOption::createHeader(std::string const &_path) { return (ok); };
MethodStatus	MethodOption::sendBody(int socket) { return (ok); };
MethodStatus	MethodOption::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};


MethodStatus		MethodOption::sendResponse(int socket) {
	return ok;
}