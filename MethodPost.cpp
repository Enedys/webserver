#include "MethodPost.hpp"

MethodPost::~MethodPost()
{

};

MethodStatus	MethodPost::createHeader()
{
	return (ok);
};

MethodStatus MethodPost::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
//	static std::string body;
	static int  var = 0;
	// std::cout << "BOOOODY: " << requestBody << std::endl;
	var += requestBody.length();
	std::cout << "BODY: " << requestBody << "\n";
	if (bodyStatus == inprogress)
		return (inprogress);
	std::cout << "size: " << var << std::endl;
	var = 0;
	return (ok);
};

MethodStatus	MethodPost::manageRequest()
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
