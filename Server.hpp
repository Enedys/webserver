#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"

class Server
{
private:
	int					_socket;		/* file descriptor analog for network applications*/
	struct sockaddr_in 	_sockAddr;		/* c/c++ structure for sockets */
	t_ext_serv			_server;
	void				closeSocket();
	Server();
public:
	Server(t_ext_serv const &config);
	int					createSocket();
	int					getServePort() const;
	int					getServerSocket() const;
	t_serv const		&getDefaultServer() const;
	std::string const 	&getServeHost() const;
	t_ext_serv const	&getConfig() const;
	Server	&operator=(const Server &s);
	~Server();
};

std::ostream		&operator<<(std::ostream &o, const Server &s);
