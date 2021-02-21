#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

class MethodPost: public AMethod
{
private:
    /* data */
	struct uri
	{
		std::string path;
		char **env;
		int pathFd;
		int socket;
		s_loc loc;
	};
		uri _uri;
		bool _inProgress;
		char **env;
    MethodPost(/* args */);
    void createCgiEnv();
public:
	MethodPost(t_serv const &config, int &status, stringMap const &headers) :
		AMethod(config, status, headers) {_type = POST; _inProgress = false; };
	virtual ~MethodPost();
	virtual MethodStatus	createHeader(std::string const &_path);
	virtual MethodStatus	processBody(const std::string &requestBody);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	manageRequest(std::string const &location);
	virtual MethodStatus	sendBody(int socket);
};
