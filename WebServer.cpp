#include "WebServer.hpp"

WebServer::WebServer()
{
	_webLogger << Message::sys <<"Create WebServer without servers." << Logger::endl;
}

WebServer::~WebServer()
{
	_webLogger << Message::sys << "WebServer off." << Logger::endl;
}

WebServer::WebServer(std::list<Server *> serverList) : _serverList(serverList)
{
	_webLogger << Message::sys << "Create WebServer without servers." << Logger::endl;
}

void		WebServer::appendServer(Server *newServer)
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

WebServer::client					&WebServer::detachConnection(client &cIt)
{
	_webLogger << Message::warning << "Connection closed. socket: " << (*cIt)->getClientSocket() << Logger::endl;
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
		_webLogger << Message::verbose << "Listen server socket: " << (*sIt)->getServerSocket() << Logger::endl;
		FD_SET((*sIt)->getServerSocket(), &_fdsToRead);
		sIt++;
	}
	while (cIt != _clientList.end())
	{
		if ((*cIt)->getRequestStatus() == Request::responseReady)
		{
			std::cout << "\033[32mwHERE!\033[0m\n";
			FD_SET((*cIt)->getClientSocket(), &_fdsToWrite);
			_webLogger << Message::verbose << "Client socket to write: " << (*cIt)->getClientSocket() << Logger::endl;
		}
		if ((*cIt)->getRequestStatus() != Request::responseReady)
		{
			std::cout << "\033[35mrHERE!\033[0m\n";
			_webLogger << Message::verbose << "Listen client socket: " << (*cIt)->getClientSocket() << Logger::endl;
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
		if (FD_ISSET((*sIt)->getServerSocket(), &_fdsToRead))
		{
			_webLogger << Message::request << "New connection request to "<< (*sIt)->getServerSocket() << Logger::endl;
			int	newSocket = accept((*sIt)->getServerSocket(),\
			reinterpret_cast<sockaddr *>(&clientName), &clientLen);
			if (newSocket == -1)
			{
				_webLogger << Message::warning << "Connection refused."<< (*sIt)->getServerSocket() << Logger::endl;
				return (1);
			}
			fcntl(newSocket, F_SETFL, O_NONBLOCK);
			_clientList.push_back(new Client(newSocket, clientName, (*sIt)->getConfigPtr()));
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
			_webLogger << Message::verbose << "Read request from: "\
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
			_webLogger << Message::verbose << "Send response to socket: "\
				<< (*cIt)->getClientSocket() << Logger::endl;
			/* cIt->sendResponse() must be here */
			send((*cIt)->getClientSocket(), resp, sizeof(resp) - 1, MSG_DONTWAIT);
			send((*cIt)->getClientSocket(), bodya, sizeof(bodya) - 1, MSG_DONTWAIT);
			(*cIt)->setRequestStatus(Request::none); // setWaitRequestStatus();
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
			_webLogger << Message::error <<"Select error." << Logger::endl;
		_webLogger << Message::fatal << "..." << Logger::endl;
		usleep(1000000);
	}
}
