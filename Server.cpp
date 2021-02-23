#include "Server.hpp"

Server::Server() {};
Server::~Server() {};
void				Server::closeSocket() {if (_socket != -1) close(_socket);};
t_serv const		&Server::getDefaultServer() const {return (_server.servs[0]);}
int					Server::getServePort() const {return (_server.port);};
std::string const 	&Server::getServeHost() const {return (_server.host);};
t_ext_serv const	&Server::getConfig() const { return (_server);}
int					Server::getServerSocket() const {return (_socket);};


Server	&Server::operator=(const Server &s)
{
	closeSocket();
	_server = s._server;
	createSocket();		/* Exception may be thrown here */
	return (*this);
}

std::ostream		&operator<<(std::ostream &o, const Server &s)
{
	o << "host: " << s.getServeHost() << ", port: " << \
	s.getServePort() << ", socket: " << s.getServerSocket();
	return (o);
}

Server::Server(t_ext_serv const &conf) try : _server(conf)
{
	createSocket();
}
catch(char const *s)
{
	std::cout << s << std::endl;
};

int					Server::createSocket()
{
	int yes = 1;
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw ("Socket creation error");
	fcntl(_socket, F_SETFL, O_NONBLOCK);
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)		/* for reusing _socket with same id */
		throw ("Socket options error.");
	memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	if  (_server.host == "localhost")
		_sockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else if (_server.host == "")
		_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else if ((_sockAddr.sin_addr.s_addr = inet_addr(_server.host.c_str()))\
				== INADDR_NONE)
		throw ("Server addres error");
	_sockAddr.sin_port = htons(_server.port);
	if (bind(_socket, reinterpret_cast<sockaddr *>(&_sockAddr), sizeof(_sockAddr)))	/* set corresponding beteween socket and server ip:port */
		throw ("Bind error.");
	if (listen(_socket, 10))														/* open socket for listening and set max connections number for current server */
		throw ("Listen failed.");
	return (1);
	
}
