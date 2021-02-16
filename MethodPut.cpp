#include "MethodPut.hpp"

MethodPut::~MethodPut() {};
MethodStatus	MethodPut::createHeader() { return (ok); };
MethodStatus	MethodPut::createErrorHeader() { return (ok); };
MethodStatus	MethodPut::sendHeader(int socket) { return (ok); };
MethodStatus	MethodPut::readBody(int socket) { return (ok); };

MethodStatus	MethodPut::processRequest(std::string	const &location) {

	// tryToPost();// PUT /new/post

	//// if Success >> HTTP/1.1 201 Created + Content-Location Header:
	// createGeneralHeaders();
	// addLocationHeader();

	return (ok);
};

MethodStatus	MethodPut::sendBody(int socket) { return (ok); };
