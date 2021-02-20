#pragma once
#include "include_resources.hpp"
#include "Server.hpp"
#include "NewClient.hpp"

class WebServer
{
public:
	typedef std::list<Client *>::iterator		client;
	typedef std::list<Server *>::iterator		server;

	WebServer(std::list<Server *> serverList);
	WebServer();
	~WebServer();
	
	void				appendServer(Server *newServer);
	void				showServerList();
	int					removeServer();
	int					runWebServer();
private:
	Logger					_webLogger;
	std::list<Server *>		_serverList;
	std::list<Client *>		_clientList;
	fd_set					_fdsToRead;
	fd_set					_fdsToWrite;
	fd_set					_actualFds;
	int						max_fd;
	bool					isReadMod;

	int						checkActualConnections();
	int						acceptNewConnections();
	int						setActualConnections();
	int						readActualRequests();
	int						sendActualResponses();
	int						communicate();
	client					&detachConnection(client &cIt);
};
