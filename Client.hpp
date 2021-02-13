#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"
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

	Client(int socket, const struct sockaddr_in &sockAddr, t_serv const *config);
	virtual ~Client();
};
