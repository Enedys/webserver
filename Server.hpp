#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"

typedef std::vector<s_loc>::iterator		locIter;
typedef std::vector<s_loc>::const_iterator	constLocIter;

class Server
{
private:
	int					_socket;		/* file descriptor analog for network applications*/
	struct sockaddr_in 	_sockAddr;		/* c/c++ structure for sockets */
	t_ext_serv			_server;
	void				closeSocket();
	Server();
public:
	int					createSocket();
	int					getServePort() const;
	int					getServerSocket() const;
	t_serv const		&getDefaultServer() const;
	std::string const 	&getServeHost() const;
	t_ext_serv const	&getConfig() const;


	static t_serv const *determineServer(t_ext_serv const &servList, std::string const &host);
	static s_loc const	*findLocation(t_serv const *serv, std::string const &script_name);
	static int			match(std::string const &s1, std::string const &s2, size_t i1, size_t i2);


	Server(t_ext_serv const &config);
	Server	&operator=(const Server &s);
	~Server();
};

std::ostream		&operator<<(std::ostream &o, const Server &s);
