#pragma once
#include "Server.hpp"
#include "Client.hpp"
#include <sys/select.h>
#include <list>

typedef std::list<Client>::iterator client;
typedef std::list<Server>::iterator server;
class WebServer
{
private:
	std::list<Server>		_serverList;
	std::list<Client>		_clientList;
	fd_set					_fdsToRead;
	fd_set					_fdsToWrite;
	fd_set					_actualFds;
	int						_fdMax;
	int						setActualConnections();
	int						checkActualConnections();
	int						readActualRequests();
	int						acceptNewConnections();
	client					&detachConnection(client &cIt);
	int						getRequest(client &cIt);
public:
	WebServer();
	void				appendServer(const Server &newServer);
	void				showServerList();
	int					removeServer();
	int					runWebServer();

	/* This fucntion must be realize into Request class */

	WebServer(std::list<Server> serverList);
	~WebServer();
};

WebServer::WebServer() : _fdMax(0)
{
	std::cout << "Create server with empty servers." << std::endl;
}

WebServer::~WebServer()
{
}

WebServer::WebServer(std::list<Server> serverList) : _serverList(serverList), _fdMax(0)
{
	std::cout << "Create server using servers list." << std::endl;
}

void		WebServer::appendServer(const Server &newServer)
{
	/* Bad allocation case */
	_serverList.push_back(newServer);
	std::cout << "New server was added to server list and currently has "\
	<< _serverList.size() << " elements." << std::endl;
}

void				WebServer::showServerList()
{
	server	it = _serverList.begin();
	while (it != _serverList.end())
	{
		std::cout << *it << std::endl;
		it++;
	}
}

client					&WebServer::detachConnection(client &cIt)
{
	close(cIt->getClientSocket());
	FD_CLR(cIt->getClientSocket(), &_fdsToRead);
	cIt = _clientList.erase(cIt);
	return (cIt);
}

int						WebServer::getRequest(client &cIt)
{
	std::stringstream	msg;
	char	buffer[1024];
	bzero(buffer, 1024);
	int	read = 0;
	while ((read = recv(cIt->getClientSocket(), buffer, 1023, MSG_DONTWAIT)) > 0)
	{
		std::cout << "getRequest::Read request\n";
		buffer[read] = '\0';
		msg << buffer;
	}
	if (msg.str().empty() && read == 0)
	{
		std::cout << "End of the connection.\n";
		return (1);
	}
	std::cout << "\033[32m Message \033[0m \n" << msg.str() << std::endl;
	return (0);
}

int						WebServer::setActualConnections()
{
	FD_ZERO(&_fdsToRead);
	FD_ZERO(&_fdsToWrite);
	server	sIt = _serverList.begin();
	client	cIt = _clientList.begin();
	while (sIt != _serverList.end())
	{
		std::cout << "Set socket: " << sIt->getServerSocket() << std::endl;
		FD_SET(sIt->getServerSocket(), &_fdsToRead);
		_fdMax = sIt->getServerSocket();
		sIt++;
	}
	while (cIt != _clientList.end())
	{
		FD_SET(cIt->getClientSocket(), &_fdsToRead);
		std::cout << "Client socket: " << cIt->getClientSocket() << " set to read\n";
		_fdMax = cIt->getClientSocket();
		cIt++;
	}
	return (0);
}

int						WebServer::checkActualConnections()
{
	if (select(_fdMax + 1, &_fdsToRead, NULL, NULL, NULL) == -1)
		return (-1);
	return (0);
}

int						WebServer::acceptNewConnections()
{
	server	sIt = _serverList.begin();
	struct sockaddr_in			clientName;
	socklen_t					clientLen = sizeof(clientName);
	int							newSocket;
	while (sIt != _serverList.end())
	{
		bzero(&clientName, clientLen);
		if (FD_ISSET(sIt->getServerSocket(), &_fdsToRead))
		{
			std::cout << "Server socket: "<< sIt->getServerSocket() << std::endl;
			int	newSocket = accept(sIt->getServerSocket(),\
			reinterpret_cast<sockaddr *>(&clientName), &clientLen);
			if (newSocket == -1)
				throw ("Acception error.");
			fcntl(newSocket, F_SETFL, O_NONBLOCK);
			_clientList.push_back(Client(newSocket, clientName));
			std::cout << "New connection was created.\n";
		}
		sIt++;
	}
	return (0);
}

int						WebServer::readActualRequests()
{
	client	cIt = _clientList.begin();
	int		resReq = 0;
	while (_clientList.end() != cIt)
	{
		if (FD_ISSET(cIt->getClientSocket(), &_fdsToRead))
		{
			std::cout << "Read request.\n";
			if (!(resReq = getRequest(cIt)))
			{
				cIt = detachConnection(cIt);
				if (resReq == 1)
					throw ("Error with reading request.");
				continue ;
			}
		}
		cIt++;
	}
	return (0);
}

int						WebServer::runWebServer()
{
    sockaddr_in clientAddr;
    socklen_t   clientLen = sizeof(clientAddr);

	while (1)
	{
		std::cout << "...\n";
		setActualConnections();
		if (checkActualConnections() != -1)
		{
			readActualRequests();
			acceptNewConnections();
		}
		else
			throw ("Select error.");
		usleep(1000000);
	}
}
