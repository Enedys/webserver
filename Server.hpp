#pragma once
#include "includes.hpp"
// #include <utility>
#include <map>

// typedef std::pair<std::string, std::string>	stringPair;
typedef std::map<std::string, std::string>	stringMap;
typedef std::map<std::string, std::string>::iterator	mapIter;

class Server
{
private:
    int                 _fdSocket;
    unsigned int        _port;
    std::string         _serverName;
    struct sockaddr_in  _sockAddr;
	std::string			_root;		//_locations enought to define server
	stringMap			_locations; //correspondese location -> root ?
    int                 createSocket();
    void                 closeSocket();
public:
    const std::string   &getServerName() const;
    unsigned int        getServePort() const;
    int                 getServerSocket() const;
    const std::string	getRoot() const;
    void				setRoot(const std::string &root);
	void				setServePort(unsigned int port);
    void				setServerName(const std::string &sName);
	void				appendLocation(const std::string &location, std::string const &root);
	std::string			getUri(std::string const &location);
	
	Server();
    Server(std::string serverName, unsigned int port);
    ~Server();
};

Server::Server()
{
}

void				Server::setRoot(const std::string &root){_root = root;};
void				Server::setServePort(unsigned int port) {_port = port;};
void				Server::setServerName(const std::string &sName) {_serverName = sName;}; 
void				Server::appendLocation(const std::string &location, std::string const &root="")
{
	if (root == "")
		_locations[location] = _root;
	else
		_locations[location] = root;		
}

std::string			Server::getUri(std::string const &uri)
{
	mapIter	itLoc = _locations.begin();
	mapIter	itBest = _locations.end();
	size_t	pos = 0;
	int		first = 1;
	// std::string	filename = uri.substr(uri.find_last_of('/'));
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
	if (itBest == _locations.end())
		return ("/");
	if (itBest->second == "/")
		return (uri.substr(itBest->first.length()));
	return (itBest->second + uri.substr(itBest->first.length()));
}

Server::~Server()
{
    // if (_fdSocket != -1)
    //     close(_fdSocket);
}

Server::Server(std::string serverName, unsigned int port) try :
    _serverName(serverName), _port(port)
{
    createSocket();
}
catch(std::string const &s)
{
    std::cout << s << std::endl;
};

void     Server::closeSocket()
{
    if (_fdSocket != -1)
        close(_fdSocket);
}

int    Server::createSocket()
{
	int yes = 1;
	if ((_fdSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw ("Socket creation error");
	fcntl(_fdSocket, F_SETFL, O_NONBLOCK);
	if (setsockopt(_fdSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		throw "Socket options error.";
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
	if (bind(_fdSocket, reinterpret_cast<sockaddr *>(&_sockAddr), sizeof(_sockAddr)))
		throw ("Bind error.");
	if (listen(_fdSocket, 10))
		throw ("Listen failed.");
	return (1);
}

std::string const	&Server::getServerName() const
{
	return (_serverName);
}

unsigned int		Server::getServePort() const
{
	return (_port);
}

int					Server::getServerSocket() const
{
	return (_fdSocket);
}

std::ostream		&operator<<(std::ostream &o, const Server &s)
{
	o << "host: " << s.getServerName() << ", port: " << s.getServePort()\
	<< ", socket: " << s.getServerSocket();
	return (o);
}
