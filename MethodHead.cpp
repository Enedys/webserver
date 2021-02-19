#include "MethodHead.hpp"

MethodHead::MethodHead(MethodHead const & src) {};
MethodHead & MethodHead::operator=(MethodHead const & src) {};
MethodHead::MethodHead() {};

MethodHead::MethodHead(t_serv const &config, int &code, stringMap const &headersMapRequest) {};
MethodHead::~MethodHead() {};

MethodStatus	MethodHead::readRequestBody() { return (ok); };
MethodStatus	MethodHead::manageRequest(std::string const &path) { return (ok); };
MethodStatus	MethodHead::createHeader() { return (ok); };
MethodStatus	MethodHead::sendHeader(int socket) { return (ok); };
MethodStatus	MethodHead::sendBody(int socket) { return (ok); };