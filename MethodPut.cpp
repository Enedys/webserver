#include "MethodPut.hpp"

MethodPut::MethodPut(MethodPut const & src) {};
MethodPut & MethodPut::operator=(MethodPut const & src) {};
MethodPut::MethodPut() {};

MethodPut::MethodPut(t_serv const &config, int &code, stringMap const &headersMapRequest) {};
MethodPut::~MethodPut() {};

MethodStatus	MethodPut::readRequestBody() { return (ok); };
MethodStatus	MethodPut::manageRequest(std::string const &path){
	// tryToPut();// PUT /new/post

	//// if Success >> HTTP/1.1 201 Created + Content-Location Header:
	// createGeneralHeaders();
	// addLocationHeader();

	// return (ok);
};
MethodStatus	MethodPut::createHeader() { return (ok); };
MethodStatus	MethodPut::sendHeader(int socket) { return (ok); };
MethodStatus	MethodPut::sendBody(int socket) { return (ok); };