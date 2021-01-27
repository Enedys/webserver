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

class Server
{
private:
    int                 _fdSocket;
    unsigned int        _port;
    std::string         _serverName;
    struct sockaddr_in  _sockAddr;
    int                 createSocket();
    void                 closeSocket();
public:
    const std::string   &getServerName() const;
    unsigned int        getServePort() const;
    int                 getServerSocket() const;
    Server();
    Server(std::string serverName, unsigned int port);
    ~Server();
};

Server::Server()
{
}

Server::~Server()
{
    if (_fdSocket != -1)
        close(_fdSocket);
}

Server::Server(std::string serverName, unsigned int port) try :
    _serverName(serverName), _port(port)
{
    createSocket();
}
catch(std::string const &s)
{
    std::cout << s << std::endl;
};

void     Server::closeSocket()
{
    if (_fdSocket != -1)
        close(_fdSocket);
}

int    Server::createSocket()
{
    int yes = 1;
    if ((_fdSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw ("Socket creation error");
    fcntl(_fdSocket, F_SETFL, O_NONBLOCK);
    if (setsockopt(_fdSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        throw "Socket options error.";
    bzero(&_sockAddr, sizeof(_sockAddr));
    _sockAddr.sin_family = AF_INET;
    if  (_serverName == "localhost")
        _sockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    else if (_serverName == "any")
        _sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else if ((_sockAddr.sin_addr.s_addr = inet_addr(_serverName.c_str()))\
                == INADDR_NONE)
        throw ("Server addres error");
    _sockAddr.sin_port = htons(_port);
    if (bind(_fdSocket, reinterpret_cast<sockaddr *>(&_sockAddr), sizeof(_sockAddr)))
        throw ("Bind error.");
    if (listen(_fdSocket, 10))
		throw ("Listen failed.");
    // std::cout << *this << std::endl;
    return (1);
}

std::string const   &Server::getServerName() const
{
    return (_serverName);
}

unsigned int        Server::getServePort() const
{
    return (_port);
}

int                 Server::getServerSocket() const
{
    return (_fdSocket);
}

std::ostream    &operator<<(std::ostream &o, const Server &s)
{
    o << "Server name: " << s.getServerName() << ", port: " << s.getServePort()\
    << ", socket fd: " << s.getServerSocket();
    return (o);
}