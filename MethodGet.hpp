#ifndef METHODGET_HPP
# define METHODGET_HPP

# include <iostream>
# include <sys/stat.h>

# include "Method.hpp"
# include "Response.hpp"

typedef	enum
{
	noRequest,
	noMethod,
	errorReadBody = 400,
	errorProcessBody = 400,
	okWaitingMoreChunks = 400,
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

class methodGet : public IMethod
{
private:
	OurHeader		*_header;
	// Response	*_response;//std::string
	int				_socket;
	int				_status;
	std::string		*_body;//
	// void		*_body;
	// methodGet methodGet(methodGet & const src);
	// methodGet & operator=(methodGet & const src);
	methodGet();

public:
	methodGet(int socket, t_serv *config);//if status not ok don't call MethodGet in Response
	~methodGet();

	int			getStatus() const { return this->_status; };//will be retrieved from Response
	OurHeader	*getHeader() const { return this->_header; };// std::map	getHeader
	std::string	*getBody() const { return this->_body; };

	int		run()
	{
		// this->createHeader();//createErrorHeader()?//senError?
		// this->sendHeader();//when

		std::string content = "";
		std::string path = getPath();//путь из хедера?

		struct stat		buf;
		if (stat(path.c_str(), &buf) == -1){
			std::cout << "Cannot open file.\n";//file does not exist
			return ;//error 202?
		}

		std::ifstream	file;
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return ;//403
		// content = readContent(path);
		content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());///
		file.close();

		return (200);
	}
};

methodGet::methodGet(){};

methodGet::~methodGet(){};

// int		readContent(void);

{
	ResponseHeader	head;

	_code = readContent();
	_response = head.getHeader(_content, _path, _code) + _content;
}


#endif
