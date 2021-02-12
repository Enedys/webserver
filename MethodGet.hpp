#ifndef MethodGet_HPP
# define MethodGet_HPP

# include <iostream>
# include <sys/stat.h>
# include <fstream>

# include "Method.hpp"
# include "Response.hpp"

# define BUFSIZE 1024

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
	ok = 200
}		ErrorStatus;

// typedef struct	Header {
// 	std::string header;
// 	// std::map<string, string> {"header_name" : "value"}
// }				OurHeader;

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
	t_serv			*_config;
	int				_status;

	MethodGet(MethodGet & const src);
	MethodGet & operator=(MethodGet & const src);
	MethodGet();

public:
	MethodGet(t_serv *config);//if status not ok don't call MethodGet in Response
	~MethodGet();

	int	createHeader() {};
	int	createErrorHeader() {};
	int	sendHeader(int socket);
	int	sendError() {};

	// std::string	*headerToString(OurHeader *buf);// const

	int		getStatus() const { return this->_status; };//will be retrieved from Response
	int		processRequest(std::string &path);
	int		sendBody(std::string &path, int socket);
};

// int		readContent(void);

// {
// 	ResponseHeader	head;

// 	_code = readContent();
// 	_response = head.getHeader(_content, _path, _code) + _content;
// }


#endif
