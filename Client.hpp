#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "includes.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Client : public Request, public Response
{
private:
	int					_socket;
	struct sockaddr_in	_sockAddr;
	Client();
public:
	void				closeSocket();
	const std::string   &getClientName() const;
	int					getClientSocket() const;
	int					setClientSocket(int socket);

	Client(int socket, const struct sockaddr_in &sockAddr, serv_config const *config);
	virtual ~Client();
};

Client::~Client()
{
	closeSocket();
}

Client::Client(int socket, const struct sockaddr_in &sockAddr, serv_config const *config) :
Request(socket), Response(config, this), _socket(socket), _sockAddr(sockAddr)
{
}

void				Client::closeSocket()
{
}

int					Client::getClientSocket() const
{
	return (_socket);
}

int					Client::setClientSocket(int socket)
{
	if (socket > 0)
		_socket = socket;
	else
		throw "Error with setig socket into Client.";
	return (0);
}

std::ostream	&operator<<(std::ostream &o, const Client &s)
{
	o << "Client socket: " << s.getClientSocket();
	return (o);
}
