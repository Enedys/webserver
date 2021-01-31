#pragma once
#include "Server.hpp"
#include "Client.hpp"
#include "Debugger.hpp"
#include "includes.hpp"

typedef std::list<Client *>::iterator client;
typedef std::list<Server>::iterator server;
class WebServer
{
private:
	Logger					_webLogger;
	std::list<Server>		_serverList;
	std::list<Client *>		_clientList;
	fd_set					_fdsToRead;
	fd_set					_fdsToWrite;
	fd_set					_actualFds;
	int						max_fd;
	int						checkActualConnections();
	int						acceptNewConnections();
	int						setActualConnections();
	int						readActualRequests();
	int						sendActualResponses();
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
	_webLogger << warning << "Connection closed. socket: " << (*cIt)->getClientSocket() << Logger::endl;
	close((*cIt)->getClientSocket());
	FD_CLR((*cIt)->getClientSocket(), &_fdsToRead);
	cIt = _clientList.erase(cIt);
	return (cIt);
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
		if ((*cIt)->getRequestStatus() == responseReady)
		{
			std::cout << "\033[32mwHERE!\033[0m\n";
			FD_SET((*cIt)->getClientSocket(), &_fdsToWrite);
			_webLogger << verbose << "Client socket to write: " << (*cIt)->getClientSocket() << Logger::endl;
		}
		if ((*cIt)->getRequestStatus() < responseReady)
		{
			std::cout << "\033[35mrHERE!\033[0m\n";
			_webLogger << verbose << "Listen client socket: " << (*cIt)->getClientSocket() << Logger::endl;
			FD_SET((*cIt)->getClientSocket(), &_fdsToRead);
		}
		cIt++;
	}
	return (0);
}

int						WebServer::checkActualConnections()
{
	if (select(FD_SETSIZE, &_fdsToRead, &_fdsToWrite, NULL, NULL) == -1)
		return (-1);
	return (0);
}

int						WebServer::acceptNewConnections()
{
	server	sIt = _serverList.begin();
	sockaddr_in					clientName;
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
			_clientList.push_back(new Client(newSocket, clientName));
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
		if (FD_ISSET((*cIt)->getClientSocket(), &_fdsToRead))
		{
			_webLogger << verbose << "Read request from: "\
				<< (*cIt)->getClientSocket() << Logger::endl;
			if ((resReq = (*cIt)->readRequest(&_webLogger)))
			{
				cIt = detachConnection(cIt);
				continue ;
			}
		}
		cIt++;
	}
	return (0);
}


int						WebServer::sendActualResponses()
{

	char resp[] = "HTTP/1.1 200 OK\r\n"
	"Server: nginx/1.2.1\r\n"
	"Date: Sat, 08 Mar 2014 22:53:46 GMT\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 58\r\n"
	"Last-Modified: Sat, 08 Mar 2014 22:53:30 GMT\r\n\r\n";
	char bodya[] = "<html>"
	"<head>"
	"</head>"
 	"<body>"
   	"<h1>Hello World<h1>"
 	"</body>"
	"</html>";



	client	cIt = _clientList.begin();
	int		resReq = 0;
	while (_clientList.end() != cIt)
	{
		if (FD_ISSET((*cIt)->getClientSocket(), &_fdsToWrite))
		{
			_webLogger << verbose << "Send response to socket: "\
				<< (*cIt)->getClientSocket() << Logger::endl;
			/* cIt->sendResponse() must be here */
			send((*cIt)->getClientSocket(), resp, sizeof(resp) - 1, MSG_DONTWAIT);
			send((*cIt)->getClientSocket(), bodya, sizeof(bodya) - 1, MSG_DONTWAIT);
			(*cIt)->setRequestStatus(none); // setWaitRequestStatus();
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
			sendActualResponses();
			readActualRequests();
			acceptNewConnections();
		}
		else
			_webLogger << error <<"Select error." << Logger::endl;
		_webLogger << fatal << "..." << Logger::endl;
		usleep(1000000);
	}
}
