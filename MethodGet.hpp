#ifndef MethodGet_HPP
# define MethodGet_HPP

# include <iostream>
# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>

# include "Method.hpp"

class AMethod;
// class Header;
# define BUFSIZE 1024

//max body size

// class MethodGet : public AMethod, public Header
class MethodGet : public AMethod
{
private:
	MethodGet(MethodGet const & src);
	MethodGet & operator=(MethodGet const & src);
	MethodGet();

public:
	MethodGet(t_serv const *config, Request	const *_request);
	~MethodGet();

	MethodStatus	createHeader();
	MethodStatus	createErrorHeader();
	MethodStatus	sendHeader(int socket);
	MethodStatus	processRequest(std::string const &path);
	MethodStatus	sendBody(int socket);
	MethodStatus	readBody(int socket);

	// std::string	*headerToString(OurHeader *buf);// const

	typedef	enum
	{
		// errorCreateHeader = 1,
		// errorCreateErrorHeader = 1,
		// errorReadBody = 1,
		// errorProcessBody = 1,
		notFound = 404,
		errorOpeningURL = 403,//
		errorReadingURL = 1,
		errorSocket = 1,

		errorSendHeader = 1,
		okWaitingMoreChunks = 1,//222 wtf
		okSuccess = 200
	}		ErrorStatus;

};

#endif
