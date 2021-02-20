#include "MethodOption.hpp"

MethodOption::MethodOption(MethodOption const & src) {};
MethodOption & MethodOption::operator=(MethodOption const & src) {};
MethodOption::MethodOption() {};

MethodOption::MethodOption(t_serv const &config, int &code, stringMap const &headersMapRequest) {};
MethodOption::~MethodOption() {};

MethodStatus	MethodOption::readRequestBody() { return (ok); };
MethodStatus	MethodOption::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodOption::createHeader() { return (ok); };
MethodStatus	MethodOption::readRequestBody(int socket) { return (ok); };
MethodStatus	MethodOption::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodOption::sendBody(int socket) { return (ok); };
MethodStatus	MethodOption::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
