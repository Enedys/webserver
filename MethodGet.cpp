# include "MethodGet.hpp"

// MethodGet::MethodGet(t_serv const *config) : _config(config) {
MethodGet::MethodGet(t_serv const *config) : AMethod(config) {
	// check_path_validity?
};//200

MethodGet::~MethodGet(){};

// std::string		*MethodGet::headerToString(OurHeader *buf)//contentToString?
// {
// };

int		MethodGet::sendHeader(int socket) {
	// std::string *header = headerToString(_header);
	char header[] = "HTTP/1.1 200 OK\r\n"
	"Server: nginx/1.2.1\r\n"
	"Date: Sat, 08 Mar 2014 22:53:46 GMT\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 58\r\n"
	"Last-Modified: Sat, 08 Mar 2014 22:53:30 GMT\r\n\r\n";

	// if (send(_socket, (const void *)header, header->size(), 0) < 0)//reinterpret
	// if (send(_socket, reinterpret_cast<const void *>(header), header->length(), 0) < 0)//reinterpret
	if (send(socket, reinterpret_cast<const void *>(header), strlen(header), 0) < 0){//reinterpret
		_status = errorSendHeader;
		return error;
	}

	return ok;
};

int		MethodGet::processRequest(std::string &path) {
	struct stat	st;
	if (stat(path.c_str(), &st) == -1){//lstat?
		_status = notFound;
		return error;
	}

	if ((_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK)) <= 0){
		_status = errorOpeningURL;
		return error;
	}

	// std::ifstream body(path);
	// body.open(path.c_str());
	// if (!body.is_open()){
	// 	_status = errorOpeningURL;
	// 	return error;
	// }

	return ok;
};

int		MethodGet::sendBody(int socket) {
// check_socket here too?
	size_t	ret;
	char	buf[BUFSIZE];

	while (ret = read(_fd, buf, BUFSIZE) > 0){// >= 0 //
		// int sent = send(socket, buf, ret, 0);//write O_NONBLOCK
		int sent = write(socket, buf, ret);//, O_NONBLOCK);
		if (sent == 0)//
			break ;
		if (sent < 0){
			_status = errorSocket;
			return error;
		}
	}
	if (ret == -1){
		_status = errorReadingURL;
		return error;
	}
	close(_fd);
	_status = 200;
	return ok;
}
