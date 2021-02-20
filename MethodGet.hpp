#pragma once
#include "include_resources.hpp"
#include "Method.hpp"

# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>

# include "Method.hpp"

class AMethod;
// class Header;
# define BUFSIZE 4096

//max body size

// class MethodGet : public AMethod, public Header
class MethodGet : public AMethod
{
private:
	MethodGet(MethodGet const & src);
	MethodGet & operator=(MethodGet const & src);
	MethodGet();

public:
	MethodGet(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodGet();

	virtual MethodStatus	readRequestBody();	//+path for cgi
	virtual MethodStatus	manageRequest(std::string const &path);
	virtual MethodStatus	createHeader(std::string const &_path);
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	sendBody(int socket);
	// virtual MethodStatus	sendResponse(int socket);
};

#endif

// class MethodGet: public AMethod
// {
// private:
//     /* data */
// 	MethodGet(/* args */);
// public:
// 	MethodGet(t_serv const &config, int &status, stringMap const &headers) :
// 		AMethod(config, status, headers) {};
// 	~MethodGet();
// 	MethodStatus	createHeader();
// 	MethodStatus	readRequestBody(int socket);
// 	MethodStatus	sendHeader(int socket);
// 	MethodStatus	manageRequest(std::string const &location);
// 	MethodStatus	sendBody(int socket);
// };