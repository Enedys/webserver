#pragma once
#include "include_resources.hpp"
#include "Method.hpp"
#include "CGI.hpp"
// Class RequestData:
// char **cgi_conf;
// std::string - pathToFile;
// s_loc const *location; // todo: why?
// t_serv const *serv; // todo: why?

class RequestData
{
public:
	std::string pathToFile;
	s_loc const *location;
	t_serv const *serv;
	char **cgi_conf;
}; // just because I can

class MethodPost: public AMethod
{
private:
    /* data */
    MethodPost(/* args */);
    CGI cgi;
    RequestData reqData;

public:
	MethodPost(t_serv const &config, int &status, stringMap const &headers) :
		AMethod(config, status, headers) {_type = POST;};
	virtual ~MethodPost();
	virtual MethodStatus	createHeader();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest();
	virtual MethodStatus	sendBody(int socket);
};
