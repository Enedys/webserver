#pragma once
#include "include_resources.hpp"
#include "Method.hpp"
#include "CGI.hpp"
// Class RequestData:
// char **cgi_conf;
// std::string - pathToFile;
// s_loc const *location; // todo: why?
// t_serv const *serv; // todo: why?

// class RequestData
// {
// public:
// 	std::string pathToFile;
// 	s_loc const *location;
// 	t_serv const *serv;
// }; // just because I can

class MethodPost: public AMethod
{
private:
    /* data */
    MethodPost(/* args */);
    CGI cgi;
    std::string outputString;
    // RequestData const &reqData;

public:
	MethodPost(int &status, RequestData const &data) :
		AMethod(status, data) {_type = POST;};
	virtual ~MethodPost();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendBody(int socket);
};
