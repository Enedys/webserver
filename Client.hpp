#pragma once
#include <string>
#include <iostream>
#include <sstream>

#include "sys/types.h"
#include "sys/socket.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include "fcntl.h"
#include "unistd.h"

class Client
{
private:
    int                 _fdSocket;
    struct sockaddr_in  _sockAddr;
public:
    const std::string   &getClientName() const;
	void				closeSocket();
    int                 getClientSocket() const;
	int					setClientSocket(int socket);
    Client();
    Client(int socket, const struct sockaddr_in &sockAddr);
    ~Client();
};

Client::Client()
{
}

Client::~Client()
{
	closeSocket();
}

Client::Client(int socket, const struct sockaddr_in &sockAddr) :
_fdSocket(socket), _sockAddr(sockAddr)
{
}

void     Client::closeSocket()
{
}

int                 Client::getClientSocket() const
{
    return (_fdSocket);
}

int					Client::setClientSocket(int socket)
{
	if (socket > 0 && socket < 1025)
		_fdSocket = socket;
	else
		throw "Error with setig socket into Client.";
	return (0);
}

std::ostream    &operator<<(std::ostream &o, const Client &s)
{
    o << "Client socket: " << s.getClientSocket();
    return (o);
}