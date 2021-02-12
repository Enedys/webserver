#pragma once
#include "includes.hpp"
#include "Method.hpp"
#include "Request.hpp"
#include "Parser.hpp"

typedef	enum
{
	defaultStatus,
	noRequest,
	noMethod,
	invalidRequest,
}		responseStatus;

typedef	enum
{
	defaultState,
	readingBody,
	processingResponse,
	sendingHeader,
	sendingBody,
	finishState,
	// headerError,
}		stageStatus;

typedef	int	methodStatus;

typedef Parser::t_serv	t_serv;

class Response
{
private:
	t_serv const	*_config;
	int				_socket;
	Request	const	*_request;
	AMethod			*_method;
	stageStatus		_stage;
	int				setMethod();
	ResponseStatus	sendResponse();
	int				setRequest();
	std::string		getLocation();
	Response();
	/* data */
public:
	Response(const t_serv *conf, Request const *request);
	~Response();
};

ResponseStatus		Response::sendResponse()
{
	if (!_request)
		return (noRequest);
	setMethod();
	if (!_method || _socket == -1)
		return (noMethod);
	int		res = 0;
	switch(_stage)
	{
		case defaultState:
			res = _method->createHeader();
			if (res == ok)
				_stage = _readingBody;
			else if (res == error)
				_stage = 

	}

}

int				Response::setMethod()
{
	if (!_request)
		return (noRequest);
	if (_method)
		return (defaultStatus);
	if (_request->getErrorCode())
	{
		_method = new MethodError;
		return (defaultStatus);
	}
	const stringMap line = _request->getStartLine();
	constMapIter	method = line.find("method");
	if (method == line.cend())
	{
		_method = new MethodError;
		return (defaultStatus);
	}
	if (method->second == "GET")
		_method = new MethodGet;
	else if (method->second == "HEAD")
		_method = new MethodHead;
	else if (method->second == "OPTION")
		_method = new MethodOption;
	else if (method->second == "PUT")
		_method = new MethodPut;
	else if (method->second == "POST")
		_method = new MethodPost;
	else
		return (invalidRequest);
	return (defaultStatus);
}

Response::Response(const t_serv *conf, Request const *request) :
	_config(conf), _request(request), _method(NULL), _stage(defaultState)
{
	if (!_request)
		return ;
	if (_request)
		_socket = _request->getSocket();
	setMethod();
};

Response::~Response() 
{
	delete _method;
};
