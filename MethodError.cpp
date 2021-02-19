#include "MethodError.hpp"

MethodError::MethodError(MethodError const & src) {};
// MethodError & MethodError::operator=(MethodError const & src) {};
MethodError::MethodError() {};

MethodError::MethodError(t_serv const &config, int &code, stringMap const &headersMapRequest) {};
MethodError::~MethodError() {};

MethodStatus	MethodError::readRequestBody() { return (ok); };
MethodStatus	MethodError::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodError::createHeader() { return (ok); };
MethodStatus	MethodError::sendHeader(int socket) { return (ok); };
MethodStatus	MethodError::sendBody(int socket) { return (ok); };