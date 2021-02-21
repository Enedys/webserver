//Meta-Variables:
//AUTH_TYPE
//CONTENT_LENGTH
//CONTENT_TYPE
//GATEWAY_INTERFACE
//PATH_INFO
//PATH_TRANSLATED
//QUERY_STRING
//REMOTE_ADDR
//REMOTE_IDENT
//REMOTE_USER
//REQUEST_METHOD
//REQUEST_URI
//SCRIPT_NAME
//SERVER_NAME
//SERVER_PORT
//SERVER_PROTOCOL
//SERVER_SOFTWARE
// TODO: test with bash script;

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
	if (!_inProgress)
	{
		int pos;
		pos = _uri.path.find_last_of('.');
		std::string extFile = _uri.path.substr(pos + 1, requestBody.length());
		std::string execFile = _uri.loc.cgi.find(extFile)->second;
		std::cout << "CGI method: " << execFile << std::endl;
		if (execFile.empty())
			return (error); // TODO: cgi method not found!
		_inProgress = true;
	}

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
}

void MethodPost::createCgiEnv()
{

};
