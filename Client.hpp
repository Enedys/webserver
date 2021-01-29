#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "includes.hpp"
#include "Request.hpp"

class Client : public Request
{
private:
	int					_fdSocket;
	struct sockaddr_in	_sockAddr;
	// Request				_request;
	Client();
public:
	void				closeSocket();
	const std::string   &getClientName() const;
	int					getClientSocket() const;
	int					setClientSocket(int socket);
	// int				getRequest(Logger *_webLogger);
	// bool				requestRecieved();
	// void				setWaitRequestStatus();
	Client(int socket, const struct sockaddr_in &sockAddr);
	virtual ~Client();
};

Client::~Client()
{
	closeSocket();
}

Client::Client(int socket, const struct sockaddr_in &sockAddr) :
Request(socket), _fdSocket(socket), _sockAddr(sockAddr)
{
}

void				Client::closeSocket()
{
}

int					Client::getClientSocket() const
{
	return (_fdSocket);
}

int					Client::setClientSocket(int socket)
{
	if (socket > 0)
		_fdSocket = socket;
	else
		throw "Error with setig socket into Client.";
	return (0);
}

std::ostream	&operator<<(std::ostream &o, const Client &s)
{
	o << "Client socket: " << s.getClientSocket();
	return (o);
}

// int					Client::getRequest(Logger *_webLogger)
// {
// 	return (_request.readRequest(_webLogger));
// }

// bool				Client::requestRecieved()
// {
// 	return (_request.isRecieved());
// }

// void				Client::setWaitRequestStatus()
// {
// 	_request.setStatus(false);
// }