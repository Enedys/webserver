#pragma once
#include "Server.hpp"
#include "Client.hpp"
#include "Debuger.hpp"
#include <sys/select.h>
#include <list>

typedef std::list<Client>::iterator client;
typedef std::list<Server>::iterator server;
class WebServer
{
private:
	Logger					_webLogger;
	std::list<Server>		_serverList;
	std::list<Client>		_clientList;
	fd_set					_fdsToRead;
	fd_set					_fdsToWrite;
	fd_set					_actualFds;
	int						setActualConnections();
	int						checkActualConnections();
	int						readActualRequests();
	int						acceptNewConnections();
	int						getRequest(client &cIt);
	client					&detachConnection(client &cIt);
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

WebServer::WebServer()
{
	_webLogger << sys <<"Create WebServer without servers." << Logger::endl;
}

WebServer::~WebServer()
{
	_webLogger << sys << "WebServer off." << Logger::endl;
}

WebServer::WebServer(std::list<Server> serverList) : _serverList(serverList)
{
	_webLogger << sys << "Create WebServer without servers." << Logger::endl;
}

void		WebServer::appendServer(const Server &newServer)
{
	try
	{
		_serverList.push_back(newServer);
	}
	catch(const std::exception& e)
	{
		_webLogger << error << e.what();
		return ;
	}
	_webLogger << sys << "Add server: " << newServer << Logger::endl;
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
	_webLogger << warning << "Connection closed. socket: " << cIt->getClientSocket() << Logger::endl;
	close(cIt->getClientSocket());
	FD_CLR(cIt->getClientSocket(), &_fdsToRead);
	cIt = _clientList.erase(cIt);
	return (cIt);
}

int						WebServer::getRequest(client &cIt)
{
	std::stringstream	msg;
	char	buffer[1024];
	memset(buffer, 0, 1024);
	int	read = 0;
	while ((read = recv(cIt->getClientSocket(), buffer, 1023, MSG_DONTWAIT)) > 0)
	{
		_webLogger << verbose << "Read request from socket: "\
				<< cIt->getClientSocket() << Logger::endl;
		buffer[read] = '\0';
		msg << buffer;
	}
	if (msg.str().empty() && read == 0)
	{
		_webLogger << reqread << "Сlient closed the connection" << Logger::endl;
		return (1);
	}
	std::cout << "\033[34m Message \033[0m \n" << msg.str() << std::endl;
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
		_webLogger << verbose << "Listen server socket: " << sIt->getServerSocket() << Logger::endl;
		FD_SET(sIt->getServerSocket(), &_fdsToRead);
		sIt++;
	}
	while (cIt != _clientList.end())
	{
		_webLogger << verbose << "Listen client socket: " << cIt->getClientSocket() << Logger::endl;
		FD_SET(cIt->getClientSocket(), &_fdsToRead);
		cIt++;
	}
	return (0);
}

int						WebServer::checkActualConnections()
{
	if (select(FD_SETSIZE, &_fdsToRead, NULL, NULL, NULL) == -1)
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
		memset(&clientName, 0, clientLen);
		if (FD_ISSET(sIt->getServerSocket(), &_fdsToRead))
		{
			_webLogger << request << "New connection request to "<< sIt->getServerSocket() << Logger::endl;
			int	newSocket = accept(sIt->getServerSocket(),\
			reinterpret_cast<sockaddr *>(&clientName), &clientLen);
			if (newSocket == -1)
			{
				_webLogger << warning << "Connection refused."<< sIt->getServerSocket() << Logger::endl;
				return (1);
			}
			fcntl(newSocket, F_SETFL, O_NONBLOCK);
			_clientList.push_back(Client(newSocket, clientName));
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
			_webLogger << verbose << "Read request from: "\
				<< cIt->getClientSocket() << Logger::endl;
			if (!(resReq = getRequest(cIt)))
			{
				cIt = detachConnection(cIt);
				continue ;
			}
		}
		cIt++;
	}
	return (0);
}

int						WebServer::runWebServer()
{
	sockaddr_in	clientAddr;
	socklen_t	clientLen = sizeof(clientAddr);

	while (1)
	{
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
