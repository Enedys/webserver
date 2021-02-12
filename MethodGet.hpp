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
	noRequest,
	noMethod,

	errorCreateHeader = 1,
	errorCreateErrorHeader = 1,
	errorReadBody = 1,
	errorProcessBody = 1,
	notFound = 404,
	errorOpeningURL = 403,//

	errorSendHeader = 1,
	okWaitingMoreChunks = 1,//222 wtf
	ok = 200
}		ErrorStatus;

typedef struct	Header {
	std::string header;
	// std::map<string, string> {"header_name" : "value"}
}				OurHeader;

// std::string		generateHeader(std::string content, std::string path, int code)
// {
// 	std::string	header;
// 	header = "HTTP/1.1 " + code.toStr + " " + getStatusMessage(code) + "\r\n";
// 	header += writeHeader();
// 	return (header);
// }



class MethodGet : public IMethod
{
private:
	int				_socket;
	t_serv			*_config;
	// std::string		_path;//
	int				_status;

	OurHeader		*_header;
	// Response	*_response;//std::string
	std::string		*_body;//

	MethodGet(MethodGet & const src);
	MethodGet & operator=(MethodGet & const src);
	MethodGet();

public:
	// MethodGet(int socket, t_serv *config, std::string &path);//if status not ok don't call MethodGet in Response
	MethodGet(int socket, t_serv *config);
	~MethodGet();

	int	createHeader() {};
	int	createErrorHeader() {};
	int	sendHeader();
	int	sendError() {};

	std::string	*headerToString(OurHeader *buf);// const

	int			getStatus() const { return this->_status; };//will be retrieved from Response
	OurHeader	*getHeader() const { return this->_header; };// std::map	getHeader
	std::string	*getBody() const { return this->_body; };

	int		processRequest(std::string &path);
	int		sendBody();
	// {
	// 	// this->createHeader();//createErrorHeader()?//senError?
	// 	// this->sendHeader();//when

	// 	std::string content = "";
	// 	std::string path = getPath();//путь из conf?

	// 	struct stat		buf;
	// 	if (stat(path.c_str(), &buf) == -1){
	// 		std::cout << "Cannot open file.\n";//file does not exist
	// 		return ;//error 202?404
	// 	}

	// 	std::ifstream	file;
	// 	file.open(path.c_str(), std::ifstream::in);
	// 	if (file.is_open() == false)
	// 		return ;//403
	// 	// content = readContent(path);
		content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());///
		file.close();

		return (200);
	}
};

// int		readContent(void);

// {
// 	ResponseHeader	head;

// 	_code = readContent();
// 	_response = head.getHeader(_content, _path, _code) + _content;
// }


#endif
