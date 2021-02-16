#include "Response.hpp"

int								Response::setRequest(Request const *request)
{
	_request = request;
	if (_request)
		return (0);
	return (1);
}

std::string						Response::getPath(std::string const &uri)
{
	constLocIter	itLoc = _config->locs.begin();
	constLocIter	itBest = _config->locs.end();
	size_t	pos = 0;
	// std::string	filename = uri.substr(uri.find_last_of('/'));					/* may be useful */
	// size_t	pos = uri.find_last_of('/'); //possible case delim==npos?
	// std::string	path = uri.substr(0, pos + 1);
	while (itLoc != _config->locs.end())
	{
		std::cout << "Current location: " << itLoc->path << " URI: " << uri <<std::endl;
		if ((pos = uri.find(itLoc->path)) == std::string::npos)
		{
			itLoc++;
			continue ;
		}
		std::cout << "!!!\n";
		if (itBest == _config->locs.end())
			itBest = itLoc;
		else if (itLoc->path.length() >= itBest->path.length())
			itBest = itLoc;
		itLoc++;
	}
	if (itBest == _config->locs.end())													/* what path should return if location for such uri does not exist ? */
		return ("/");
	std::string	resString;
	if (itBest->root == "/")
		return (uri.substr(itBest->path.length()));
	return (itBest->root + uri.substr(itBest->path.length()));
}


Response::responseStatus		Response::sendResponse()
{
	if (!_request)
		return (noRequest);
	setMethod();
	if (!_method || _socket == -1)
		return (noMethod);
	MethodStatus	res;
	std::string		path;
	std::cout << "IN SEND!" << _stage << std::endl;
	switch(_stage)
	{
		case defaultState:
			res = _method->createHeader();
			if (res == ok)
			{
				_stage = readingBody;
				std::cout << "OK!" << std::endl;
			}
			else if (res == error)
			{
				_stage = errorHeader;
				std::cout << "ne OK!" << std::endl;
			}
		case readingBody:
			res = _method->readBody(_socket);
			if (res == ok)
				_stage = processingResponse;
			else if (res == error)
				_stage = errorHeader;
		case processingResponse:
			std::cout << "ProcResp\n";
			path = getPath(_request->getStartLine().find("uri")->second);
			std::cout << "Path: " << path << std::endl;
			res = _method->processRequest(path); ////
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
			std::cout << "Sending header | socket:  " << _socket << "\n";
			res = _method->sendHeader(_socket);
			if (res == error)
				return (sendingError); //proposed that we drop the connection
			else if (res == ok)
				_stage = sendingBody; // what about errorHeader (what body we need to send)?
			std::cout << "Ok?\n";
		case sendingBody:
			res = _method->sendBody(_socket);
			if (res == error)
				return (sendingError);	//same logic as sendingHeaderError
			else if (res == ok)
				_stage = finishState;
			return (defaultStatus);
		case finishState:
			_stage = defaultState;
			return (defaultStatus);
	}
	return (defaultStatus);
}

int				Response::setMethod()
{
	std::cout << "Request ptr: " << _request << " Method ptr: " << _method << std::endl; 
	if (!_request)
		return (noRequest);
	if (_socket == -1)
		_socket = _request->getSocket();
	if (_method && _stage != defaultState)
		return (defaultStatus);
	if (defaultState && _method)
		delete _method;
	std::cout << "PTR: " << _method << std::endl;

	// _method = new MethodError(_config, _request); //

	if (_request->getErrorCode())
	{
		_method = new MethodError(_config, _request);
		_stage = errorHeader;
		std::cout << "Error inside errrorcode." << std::endl;
		return (defaultStatus);
	}
	const stringMap line = _request->getStartLine();
	constMapIter	method = line.find("method");
	if (method == line.cend())
	{
		std::cout << "Error noMethod" << std::endl;
		_method = new MethodError(_config, _request);
		_stage = errorHeader;
		return (defaultStatus);
	}
	if (method->second == "GET")
	{
		std::cout << "CREATE GET METHOD\n";
		_method = new MethodGet(_config, _request);
	}
	else if (method->second == "HEAD")
		_method = new MethodHead(_config, _request);
	else if (method->second == "OPTION")
		_method = new MethodOption(_config, _request);
	else if (method->second == "PUT")
		_method = new MethodPut(_config, _request);
	else if (method->second == "POST")
		_method = new MethodPost(_config, _request);
	else
	{
		_method = new MethodError(_config, _request);
		return (invalidRequest);
	}
	return (defaultStatus);
}

Response::Response(const t_serv *conf, Request const *request) :
	_config(conf), _request(request), _method(NULL), _stage(defaultState), _socket(request->getSocket())
{
	// if (!_request)
	// 	return ;
	// // setMethod();
};

Response::~Response() 
{
	delete _method;
};
