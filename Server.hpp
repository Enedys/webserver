#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"

class Server
{
private:
	int					_socket;		/* file descriptor analog for network applications*/
	struct sockaddr_in 	_sockAddr;		/* c/c++ structure for sockets */

	unsigned int		_port;			/* ordinary network port http://blabla:port/ */
	std::string			_serverName;	/* ip address of the listening server '127.0.0.1' by default */
	std::string			_root;			/* root directory of the server. like in nginx configs */
	stringMap			_locations;		/* also like in nginx. location to translate requset uri into directory on server */

	t_serv				_server;
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

	/* Actual approach to server creation */
	Server(t_serv const &config);
	t_serv const		&getConfig() const;
	int					createSocketByStruct();
};

std::ostream		&operator<<(std::ostream &o, const Server &s);
