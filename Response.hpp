#pragma once
#include "includes.hpp"
#include "Method.hpp"
#include "Request.hpp"
#include "Parser.hpp"

typedef Parser::t_serv	t_serv;

class Response
{
	public:
	Response(const t_serv *conf, Request const *request);
	~Response();
	typedef		enum
	{
		defaultStatus,
		noRequest,
		noMethod,
		invalidRequest,
		sendingError,
	}			responseStatus;
	typedef		enum
	{
		defaultState,
		readingBody,
		processingResponse,
		sendingHeader,
		sendingBody,
		finishState,
		errorHeader,
	}			stageStatus;

private:
	t_serv const	*_config;
	int				_socket;
	Request	const	*_request;
	AMethod			*_method;
	stageStatus		_stage;
	int				setMethod();
	responseStatus	sendResponse();
	int				setRequest();
	std::string		getLocation();
	Response();
};

Response::responseStatus		Response::sendResponse()
{
	if (!_request)
		return (noRequest);
	setMethod();
	if (!_method || _socket == -1)
		return (noMethod);
	MethodStatus res;
	switch(_stage)
	{
		case defaultState:
			res = _method->createHeader();
			if (res == ok)
				_stage = readingBody;
			else if (res == error)
				_stage = errorHeader;
		case readingBody:
			res = _method->readBody(_socket);
			if (res == ok)
				_stage = processingResponse;
			else if (res == error)
				_stage = errorHeader;
		case processingResponse:
			res = _method->processRequest("location");
			if (res == ok)
				_stage = sendingHeader;
			else if (res == error)
				_stage = errorHeader;
		case errorHeader:
			res = _method->createErrorHeader(); //set file fd to -1, to exclude sending body	!!!
			if (res == ok)
				_stage = sendingHeader;
			else if (res == error)
				; // how its possible?
		case sendingHeader:
			res = _method->sendHeader(_socket);
			if (res == error)
				return (sendingError); //proposed that we drop the connection
			else if (res == ok)
				_stage = sendingBody; // what about errorHeader (what body we need to send)?
		case sendingBody:
			res = _method->sendBody(_socket);
			if (res == error)
				return (sendingError);	//same logic as sendingHeaderError
			else if (res == ok)
				_stage = defaultStage;
			return (defaultStatus);
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
		_stage = errorHeader;
		return (defaultStatus);
	}
	const stringMap line = _request->getStartLine();
	constMapIter	method = line.find("method");
	if (method == line.cend())
	{
		_method = new MethodError;
		_stage = errorHeader;
		return (defaultStatus);
	}
	if (method->second == "GET")
		_method = new MethodGet();
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
