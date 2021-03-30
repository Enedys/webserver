#pragma once
#include "Method.hpp"
#include "CGI.hpp"
#include "Header.hpp"
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

class MethodPost : public AMethod
{
private:
    /* data */
    MethodPost(/* args */);
    CGI cgi;
	Header				*_header;
    std::string 		outputBuf;
	std::string			_body;//ololo
	MethodStatus	sendError(int socket);
	bool fileExists (char *filename);

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
	virtual MethodStatus	sendResponse(int socket);
};
