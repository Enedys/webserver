#include "Client.hpp"

Client::~Client() { closeSocket(); }

Client::Client(int socket, const struct sockaddr_in &sockAddr, s_serv const *config) :
Request(socket), Response(config, this), _socket(socket), _sockAddr(sockAddr)
{
}

void				Client::closeSocket() {};
int					Client::getClientSocket() const { return (_socket); };

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
