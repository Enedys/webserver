#include "Server.hpp"

Server::Server() {};
Server::~Server() {};
void				Server::closeSocket() {if (_socket != -1) close(_socket);};
std::string const	&Server::getServerName() const {return (_serverName);}
unsigned int		Server::getServePort() const {return (_port);};
t_serv const		&Server::getConfig() const { return (_server);}
int					Server::getServerSocket() const {return (_socket);};
void				Server::setRoot(const std::string &root){_root = root;};
void				Server::setServePort(unsigned int port) {_port = port;};
void				Server::setServerName(const std::string &sName) {_serverName = sName;};
void				Server::appendLocation(const std::string &location, std::string const &root="")
{
	if (root == "")
		_locations[location] = _root;
	else
		_locations[location] = root;												/* must be carefull here. for correct working ROOT for server must be set first */
}

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
	if  (_serverName == "localhost")
		_sockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else if (_serverName == "any")
		_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else if ((_sockAddr.sin_addr.s_addr = inet_addr(_serverName.c_str()))\
				== INADDR_NONE)
		throw ("Server addres error");
	_sockAddr.sin_port = htons(_port);
	if (bind(_socket, reinterpret_cast<sockaddr *>(&_sockAddr), sizeof(_sockAddr)))	/* set corresponding beteween socket and server ip:port */
		throw ("Bind error.");
	if (listen(_socket, 10))														/* open socket for listening and set max connections number for current server */
		throw ("Listen failed.");
	return (1);
}

Server::Server(std::string serverName, unsigned int port) try :
		_serverName(serverName), _port(port), _root("")
{
	createSocket();
}
catch(char const *s)
{
	std::cout << s << std::endl;
};

Server	&Server::operator=(const Server &s)
{
	_port = s._port;
	_serverName = s._serverName;
	_root = s._root;
	_locations = s._locations;


	_server = s._server;
	closeSocket();
	createSocket();																/* Exception may be thrown here */
	return (*this);
}

std::ostream		&operator<<(std::ostream &o, const Server &s)
{
	o << "host: " << s.getServerName() << ", port: " << \
	s.getServePort() << ", socket: " << s.getServerSocket();
	return (o);
}

/* ***************************************** */

Server::Server(t_serv const &conf) try : _server(conf)
{
	createSocketByStruct();
}
catch(char const *s)
{
	std::cout << s << std::endl;
};

int					Server::createSocketByStruct()
{
	int yes = 1;
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw ("Socket creation error");
	fcntl(_socket, F_SETFL, O_NONBLOCK);
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)		/* for reusing _socket with same id */
		throw ("Socket options error.");
	memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	if  (_server.serverName == "localhost")
		_sockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else if (_server.serverName == "any")
		_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else if ((_sockAddr.sin_addr.s_addr = inet_addr(_server.serverName.c_str()))\
				== INADDR_NONE)
		throw ("Server addres error");
	_sockAddr.sin_port = htons(_server.port);
	if (bind(_socket, reinterpret_cast<sockaddr *>(&_sockAddr), sizeof(_sockAddr)))	/* set corresponding beteween socket and server ip:port */
		throw ("Bind error.");
	if (listen(_socket, 10))														/* open socket for listening and set max connections number for current server */
		throw ("Listen failed.");
	return (1);
	
}
