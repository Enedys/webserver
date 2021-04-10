#include "Server.hpp"

Server::Server() {}
Server::~Server() {}
void				Server::closeSocket() {if (_socket != -1) close(_socket);}
t_serv const		&Server::getDefaultServer() const {return (_server.servs[0]);}
int					Server::getServePort() const {return (_server.port);}
std::string const 	&Server::getServeHost() const {return (_server.host);}
t_ext_serv const	&Server::getConfig() const { return (_server);}
int					Server::getServerSocket() const {return (_socket);}


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

int				Server::match(std::string const &s1, std::string const &s2, size_t i1 = 0, size_t i2 = 0)
{
	if (i1 != s1.length() && s2[i2] == '*' && (i2 == 0 || i2 == s2.length() - 1))
		return (match(s1, s2, i1 + 1, i2) + match(s1, s2, i1, i2 + 1));
	else if (i1 == s1.length() && s2[i2] == '*' && (i2 == s2.length() - 1))
		return (1);
	else if (s1[i1] == s2[i2] && i1 != s1.length() && i2 != s2.length())
		return (match(s1, s2, i1 + 1, i2 + 1));
	else if (s1[i1] == s2[i2] && i1 == s1.length() && i2 == s2.length())
		return (1);
	return (0);
}

t_serv const	*Server::determineServer(t_ext_serv const &servsList, std::string const &host)
{
	t_serv const	*serv;
	size_t			portPos = host.find_last_of(':');
	std::string		_hostName = host.substr(0, portPos);
	stringToLower(_hostName);
	std::vector<t_serv>::const_iterator sv = servsList.servs.end();
	for (std::vector<t_serv>::const_iterator i = servsList.servs.begin(); i < servsList.servs.end(); i++)
	{
		std::string	tmpServName = i->serverName;
		stringToLower(tmpServName);
		if (match(_hostName, tmpServName))
			if (sv->serverName.length() < i->serverName.length()\
				|| sv == servsList.servs.end())
				sv = i;
	}
	if (sv == servsList.servs.end())
		serv = &(servsList.servs[0]);
	else
		serv = &(*sv);
	return (serv);
}

s_loc const		*Server::findLocation(t_serv const *serv, std::string const &script_name)
{
	s_loc const	*location;
	if (!serv)
		return (NULL);
	std::cout << "SNAME: " << script_name << std::endl;
	constLocIter	itLoc = serv->locs.begin();
	constLocIter	itBest = serv->locs.end();
	while (itLoc != serv->locs.end())
	{
		if (script_name.find(itLoc->path) == std::string::npos)
		{
			itLoc++;
			continue;
		}
		if (itBest == serv->locs.end())
			itBest = itLoc;
		else if (itLoc->path.length() >= itBest->path.length())
			itBest = itLoc;
		itLoc++;
	}
	if (itBest == serv->locs.end()) //404
		return (NULL);
	else
		location = &(*itBest);
	std::cout << "VOTE: " << location->path << '\n';
	return (location);
}
