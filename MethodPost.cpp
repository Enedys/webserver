#include "MethodPost.hpp"

MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::createHeader(std::string const &_path)
{
	return (ok);
};

MethodStatus MethodPost::processBody(const std::string &requestBody)
{
//	static std::string body;
	return (ok);
};

MethodStatus	MethodPost::manageRequest(std::string const &path)
{
	return (ok);
};

MethodStatus	MethodPost::sendBody(int socket)
{
	return (ok);
};

MethodStatus	MethodPost::sendHeader(int socket)
{
	return (AMethod::sendHeader(socket));
};
