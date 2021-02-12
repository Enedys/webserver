# include "MethodGet.hpp"

// MethodGet::MethodGet(){};

MethodGet::MethodGet(int socket, t_serv *config) : _socket(socket), _config(config), _status(200) {
	// check_path_validity?
};//200

MethodGet::~MethodGet(){};

std::string		*MethodGet::headerToString(OurHeader *buf)//contentToString?
{
};

int		MethodGet::sendHeader() {
	std::string *header = headerToString(_header);

	// if (send(_socket, (const void *)header, header->size(), 0) < 0)//reinterpret
	if (send(_socket, reinterpret_cast<const void *>(header), header->size(), 0) < 0)//reinterpret
		_status = errorSendHeader;
	return -1;
};

int		MethodGet::processRequest(std::string &path) {
	struct stat	st;
	if (stat(path.c_str(), &st) == -1){//lstat?
		_status = notFound;
		return -1;
	}

	std::ifstream body(path);
	body.open(path.c_str());
	if (!body.is_open()){
		_status = errorOpeningURL;
		return -1;
	}
// check_socket

	if ((int fd = open(path.c_str(), O_RDONLY)) <= 0)

	char	buf[BUFSIZE];
	while (!body.eof()){
		body.read(buf, sizeof(buf));
		send(_socket, buf, BUFSIZE, 0);
	}
	body.close();
	_status = 200;

	return 0;
};

// int		MethodGet::sendBody() {
// 	char	buf[1024];
// 	while (!infile.eof())
// 	{
// 		infile.read (buf, sizeof(buf));
// 		Socket->write(buf,infile.gcount());
// 	}
// 	body.close();
// }
