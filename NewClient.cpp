#include "NewClient.hpp"

Client::Client(int socket, t_serv const &config) :
	_socket(socket), _config(config), _statusCode(0), _request(socket, _statusCode), _method(NULL), _state(defaultState)
{
	_requestCounter = 0;
	isReadMode = true;
}

Client::~Client()
{
	delete _method;
	close(_socket);
}

int					Client::getClientSocket() const {return _socket; };
void				Client::setMode(bool mode) {isReadMode = mode; };

bool				Client::isReading() const
{
	if (_state == defaultState || _state == readingHeader || _state == readRequestBody)
		return (true);
	return (false);
}

bool				Client::isSending() const
{
	if (_state == sendResponseBody || _state == sendResponseHeader)
		return (true);
	return (false);
}

MethodStatus		Client::refreshClient()
{
	delete _method;
	_method = NULL;
	_statusCode = 0;
	_request.cleanRequest();
	if (_request.getBufferResidual() > 0)
		_state = readingHeader;
	else
		_state = defaultState;

	_requestCounter = _requestCounter - 1;
	return (ok);
}

Client::conditionCode	Client::getNextState(MethodStatus status)
{
	if (status == connectionClosed)
		return (sendingErrorState);
	if ((_state == sendResponseBody || _state == sendResponseHeader) && status == error)
		return (sendingErrorState); // here must cut connection
	if (status == error)
	{
		if (_state <= analizeHeader)
			createNewMethod();
		return (createResponseHeader);
	}
	else if (status == ok)
		return (static_cast<Client::conditionCode>(static_cast<int>(_state) + 1));
	return (_state);
}

MethodStatus		Client::analizeHeaders()
{
	if (createNewMethod() != ok)
		return (error);
	return (ok);
}

std::string			Client::getRequestPath(std::string const &uri)
{
	constLocIter	itLoc = _config.locs.begin();
	constLocIter	itBest = _config.locs.end();
	size_t	pos = 0;
	while (itLoc != _config.locs.end())
	{
		if ((pos = uri.find(itLoc->path)) == std::string::npos)
		{
			itLoc++;
			continue ;
		}
		if (itBest == _config.locs.end())
			itBest = itLoc;
		else if (itLoc->path.length() >= itBest->path.length())
			itBest = itLoc;
		itLoc++;
	}
	if (itBest == _config.locs.end())													/* what path should return if location for such uri does not exist ? */
		return ("");
	if (itBest->root == "/")
		return (uri.substr(itBest->path.length()));
	return (itBest->root + uri.substr(itBest->path.length()));
}

MethodStatus		Client::createNewMethod()
{
	if (_socket == -1)
		return (socketError); // и что туту делать?
	if (_statusCode)
	{
		_method = new MethodGet(_config, _statusCode, _request.getHeadersMap());
		_state = createResponseHeader;
		return (ok);
	}
	if (_state != analizeHeader)
		return (logicError);
	const std::string method = _request.getStartLine().find("method")->second;
	if (method == "GET")
		_method = new MethodGet(_config, _statusCode, _request.getHeadersMap());
	else if (method == "HEAD")
		_method = new MethodHead(_config, _statusCode, _request.getHeadersMap());
	else if (method == "OPTION")
		_method = new MethodOption(_config, _statusCode, _request.getHeadersMap());
	else if (method == "PUT")
		_method = new MethodPut(_config, _statusCode, _request.getHeadersMap());
	else if (method == "POST")
		_method = new MethodPost(_config, _statusCode, _request.getHeadersMap());
	return (ok);
}


MethodStatus		Client::requestInterraction()
{
	if (_state == defaultState)
		_state = readingHeader;
	if (_state == readingHeader)
	{
		_state = getNextState(_request.readRequestHead(NULL));
		if (_state == readingHeader)
			return (inprogress);
	}
	if (_state == analizeHeader)
		_state = getNextState(analizeHeaders());
	if (_state == readRequestBody)
	{
		_state = getNextState(_request.readRequestBody(_method, NULL));
		if (_state == readRequestBody)
			return (inprogress);
	}
	if (_state == manageRequest)
		_state = getNextState(_method->manageRequest(\
					getRequestPath(_request.getURI())));
	if (_state == createResponseHeader)
		_state = getNextState(_method->createHeader(\
					getRequestPath(_request.getURI())));
	return (ok);
}

MethodStatus		Client::responseInsterraction()
{
	if (_state == sendResponseHeader)
	{
		_state = getNextState(_method->sendHeader(_socket));
		if (_state == sendResponseHeader)
			return (inprogress);
	}
	if (_state == sendResponseBody)
	{
		_state = getNextState(_method->sendBody(_socket));
		if (_state == sendResponseBody)
			return (inprogress);
	}
	if (_state == finalState)
		refreshClient();
	return (ok);
}

MethodStatus		Client::interract()
{
	MethodStatus	returnStatus;
	if (isReadMode)
		returnStatus = requestInterraction();
	else
		returnStatus = responseInsterraction();
	if (_state == sendingErrorState)
		return (error);
	return (returnStatus);
}
