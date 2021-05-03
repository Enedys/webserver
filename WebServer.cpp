#include "WebServer.hpp"

WebServer::WebServer()
{
	printLog(&_webLogger, "Create WebServer without servers.", -1, Message::sys);
}

WebServer::~WebServer()
{
	printLog(&_webLogger, "WebServer without servers.", -1, Message::sys);
}

WebServer::WebServer(std::list<Server *> serverList) : _serverList(serverList)
{
	printLog(&_webLogger, "Create WebServer without servers.", -1, Message::sys);
}

void				WebServer::appendServer(Server *newServer)
{
	try
	{
		_serverList.push_back(newServer);
	}
	catch(const std::exception& e)
	{
		_webLogger << Message::error << e.what();
		return ;
	}
	_webLogger << Message::sys << "Add server: " << newServer << Logger::endl;
}

void				WebServer::showServerList()
{
	server	it = _serverList.begin();
	while (it != _serverList.end())
	{
		std::cout << *(*it) << std::endl;
		it++;
	}
}

WebServer::client	&WebServer::detachConnection(client &cIt)
{
	_webLogger << Message::warning << "Connection closed. socket: " << (*cIt)->getClientSocket() << Logger::endl;
	// close((*cIt)->getClientSocket());
	FD_CLR((*cIt)->getClientSocket(), &_fdsToRead);
	FD_CLR((*cIt)->getClientSocket(), &_fdsToWrite);
	delete *cIt;
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
		_webLogger << Message::verbose << "Listen server socket: " << (*sIt)->getServerSocket() << Logger::endl;
		FD_SET((*sIt)->getServerSocket(), &_fdsToRead);
		sIt++;
	}
	while (cIt != _clientList.end())
	{
		// set all clients to read
		_webLogger << Message::verbose << "WS_set:: Client socket to read: " << (*cIt)->getClientSocket() << Logger::endl;
		FD_SET((*cIt)->getClientSocket(), &_fdsToRead);

		if ((*cIt)->readyToSend())
		{
			FD_SET((*cIt)->getClientSocket(), &_fdsToWrite);
			_webLogger << Message::verbose << "WS_set:: Client socket to write: " << (*cIt)->getClientSocket() << Logger::endl;
		}
		cIt++;
	}
	return (0);
}

int						WebServer::checkActualConnections()
{
	int		newChanges = select(FD_SETSIZE, &_fdsToRead, &_fdsToWrite, NULL, NULL);
	if (newChanges == -1)
		return (-1);
	return (0);
}

int						WebServer::acceptNewConnections()
{
	sockaddr_in		clientName;
	socklen_t		clientLen = sizeof(clientName);
	server			sIt = _serverList.begin();
	while (sIt != _serverList.end())
	{
		memset(&clientName, 0, clientLen);
		if (FD_ISSET((*sIt)->getServerSocket(), &_fdsToRead))
		{
			_webLogger << Message::request << "WS_accept:: New client request to server "<< (*sIt)->getServerSocket() << Logger::endl;
			int	newSocket = accept((*sIt)->getServerSocket(),\
			reinterpret_cast<sockaddr *>(&clientName), &clientLen);
			if (newSocket == -1)
			{
				_webLogger << Message::warning << "WS_accept:: Connection refused."<< (*sIt)->getServerSocket() << Logger::endl;
				return (1);
			}
			fcntl(newSocket, F_SETFL, O_NONBLOCK);
			_clientList.push_back(new Client(newSocket, clientName, (*sIt)->getConfig()));
		}
		sIt++;
	}
	return (0);
}

int						WebServer::	clientIsReady(client cli)
{
	if (!(*cli))
		return (0);
	int	mask = 0;
	int	sock = (*cli)->getClientSocket();
	if (FD_ISSET(sock, &_fdsToRead))
		mask |= 1;
	if (FD_ISSET(sock, &_fdsToWrite))
		mask |= 2;
	return (mask);
}

int						WebServer::checkOldConnections()
{
	client	cIt = _clientList.begin();
	int	flag = 0;
	while (_clientList.end() != cIt)
	{
		flag = clientIsReady(cIt);
		if (flag)
		{
			_webLogger << Message::verbose << "CheckOld::Communicate:: client socket: "\
				<< (*cIt)->getClientSocket() << Logger::endl;
			if ((*cIt)->interract(flag & 1, flag & 2) == error)
			{
				cIt = detachConnection(cIt);
				continue ;
			}
		}
		cIt++;
	}
	return (0);
}

int						WebServer::communicate()
{
	client	cIt = _clientList.begin();
	int	flag = 0;
	while (_clientList.end() != cIt)
	{
		flag = clientIsReady(cIt);
		if (flag)
		{
			// std::cout << "r?: " << (flag & 1) << ", w?: " << (flag & 2) << std::endl;
			_webLogger << Message::verbose << "Communicate:: client socket: "\
				<< (*cIt)->getClientSocket() << Logger::endl;
			if ((*cIt)->interract(flag & 1, flag & 2) == error)
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
	while (1)
	{
		communicate();
		setActualConnections();
		if (checkActualConnections() != -1)
			acceptNewConnections();
		else
			_webLogger << Message::error <<"Select error." << Logger::endl;
		_webLogger << Message::fatal << "..." << Logger::endl;
		// usleep(1000000);
	}
}
