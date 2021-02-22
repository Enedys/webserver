#include "MethodPost.hpp"

MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::createHeader(std::string const &_path)
{
	return (ok);
};

MethodStatus MethodPost::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
//	static std::string body;
	static int  var = 0;
	// std::cout << "BOOOODY: " << requestBody << std::endl;
	var += requestBody.length();
	if (bodyStatus == inprogress)
		return (inprogress);
	std::cout << "size: " << var;
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
