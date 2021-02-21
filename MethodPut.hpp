#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodPut: public AMethod
{
private:
    /* data */
    struct uri
	{
		std::string path;
		char **env;
		int pathFd;
		int socket;
	};
    uri _uri;
    bool _inProgress;
	MethodPut(/* args */);
public:
	MethodPut(t_serv const &config, int &status, stringMap const &headers) : 
		AMethod(config, status, headers) {_type = PUT; _inProgress = false; };
	virtual ~MethodPut();
	virtual MethodStatus	createHeader(std::string const &_path);
	virtual MethodStatus	processBody(const std::string &requestBody);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest(std::string const &location);
	virtual MethodStatus	sendBody(int socket);
};
