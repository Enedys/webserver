#ifndef MethodGet_HPP
# define MethodGet_HPP

# include <iostream>
# include <sys/stat.h>
# include <fstream>
# include <fcntl.h>//write nonblock
# include <time.h>
# include <sys/time.h>

// # include "Method.hpp"
# include "Response.hpp"

// class AMethod
# define BUFSIZE 1024


// std::string		generateHeader(std::string content, std::string path, int code)
// {
// 	std::string	header;
// 	header = "HTTP/1.1 " + code.toStr + " " + getStatusMessage(code) + "\r\n";
// 	header += writeHeader();
// 	return (header);
// }

//max body size


class MethodGet : public AMethod
{
private:
	MethodGet(MethodGet const & src);
	MethodGet & operator=(MethodGet const & src);
	// MethodGet();

public:
	MethodGet(t_serv const *config, Request	const *_request);
	~MethodGet();

	MethodStatus	createHeader();
	MethodStatus	createErrorHeader() { return ok; };
	MethodStatus	sendHeader(int socket);
	MethodStatus	processRequest(std::string &path);
	MethodStatus	sendBody(int socket);

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
	// std::string	*headerToString(OurHeader *buf);// const

	// int		getStatus() const { return this->_status; };//will be retrieved from Response
};

// int		readContent(void);

// {
// 	ResponseHeader	head;

// 	_code = readContent();
// 	_response = head.getHeader(_content, _path, _code) + _content;
// }


#endif
