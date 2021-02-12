#pragma once
#include "includes.hpp"
#include "Method.hpp"
#include "Request.hpp"
#include "Parser.hpp"

typedef	enum
{
	noRequest,
	noMethod,
}		ResponseStatus;

typedef Parser::t_serv	t_serv;

class Response
{
private:
	const t_serv	*_config;
	int				_socket;
	Request			*_request;
	IMethod			*_method;
	ResponseStatus	prepareResponse();
	int				setRequest();
	int				sendRequest();
	std::string		getLocation();
	Response();
	/* data */
public:
	Response(int socket, const t_serv *conf);
	~Response();
};

RequestStatus		Response::prepareResponse()
{
	if (!request)
		return (noRequest);
}

Response::Response(int socket, const t_serv *conf) : _socket(socket), _config(conf), _request(NULL), _method(NULL) {};
Response::~Response() {};
