#pragma once
#include "includes.hpp"
#include <map>

typedef std::map<std::string, std::string>	stringMap;
typedef std::map<std::string, std::string>::iterator	mapIter;

class Server
{
private:
	int					_socket;		/* file descriptor analog for network applications*/
	unsigned int		_port;			/* ordinary network port http://blabla:port/ */
	std::string			_serverName;	/* ip address of the listening server '127.0.0.1' by default */
	struct sockaddr_in 	_sockAddr;		/* c/c++ structure for sockets */
	std::string			_root;			/* root directory of the server. like in nginx configs */
	stringMap			_locations;		/* also like in nginx. location to translate requset uri into directory on server */
	
	int					createSocket();	/* the main function for server creation. throw (const char *) an exception if the server failed to create */
	void				closeSocket();
	Server();
public:
			/* getters and setters */
	const std::string	&getServerName() const;
	unsigned int		getServePort() const;
	int					getServerSocket() const;
	const std::string	getRoot() const;
	void				setRoot(const std::string &root);
	void				setServePort(unsigned int port);
	void				setServerName(const std::string &sName);

			/* add new location to the server. may be useful when parsing config file */
	void				appendLocation(const std::string &location,\
										std::string const &root);
	
			/* function to transform uri from http-requst into file-path on server */
	std::string			getUri(std::string const &location);

			/* coplien form */ 
	Server	&operator=(const Server &s);
	Server(std::string serverName, unsigned int port);
	// Server(const Server &s);
	~Server();
};

Server::Server() {};
Server::~Server() {};
void				Server::closeSocket() {if (_socket != -1) close(_socket);};
std::string const	&Server::getServerName() const {return (_serverName);}
unsigned int		Server::getServePort() const {return (_port);};
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

std::string			Server::getUri(std::string const &uri)							/* Itrates by the location and find it as substring into uri. */
{																					/* Remebver most longest corresponding */
	mapIter	itLoc = _locations.begin();
	mapIter	itBest = _locations.end();
	size_t	pos = 0;
	int		first = 1;
	// std::string	filename = uri.substr(uri.find_last_of('/'));					/* may be useful */
	// size_t	pos = uri.find_last_of('/'); //possible case delim==npos?
	// std::string	path = uri.substr(0, pos + 1);
	while (itLoc != _locations.end())
	{
		if ((pos = uri.find(itLoc->first)) == std::string::npos)
		{
			itLoc++;
			continue ;
		}
		if (first)
		{
			itBest = itLoc;
			first = 0;
		}
		else if (itLoc->first.length() > itBest->first.length())
			itBest = itLoc;
		itLoc++;
	}
	if (itBest == _locations.end())													/* what path should return if location for such uri does not exist ? */
		return ("/");
	if (itBest->second == "/")
		return (uri.substr(itBest->first.length()));
	return (itBest->second + uri.substr(itBest->first.length()));
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

// Server::Server(const Server &s) try :
// 	_port(s._port), _serverName(s._serverName), _root(s._root),
// 	_locations(s._locations)
// {
// 	std::cout << "COPY" << std::endl;
// 	createSocket();
// }
// catch(const char *ss)
// {
// 	std::cout << ss << std::endl;
// }

Server	&Server::operator=(const Server &s)
{
	_port = s._port;
	_serverName = s._serverName;
	_root = s._root;
	_locations = s._locations;
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
