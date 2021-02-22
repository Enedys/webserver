#include "NewClient.hpp"

Client::Client(int socket, t_serv const &config) :
	_socket(socket), _statusCode(0), _request(socket, _statusCode),\
	_config(config), _method(NULL), _state(defaultState)
{
}

Client::~Client()
{
	delete _method;
	close(_socket);
}

int					Client::getClientSocket() const
{
	return _socket;
};

bool				Client::isReading() const
{
	if (_state == defaultState || _state == readingHeader || _state == readingBody)
		return (true);
	if (_request.getRequestState() == Request::body)
		return (true);
	return (false);
}

bool				Client::readyToSend() const
{
	if (_state == sendingBody || _state == sendindHeader)
		return (true);
	return (false);
}

MethodStatus		Client::refreshClient()
{
	delete _method;
	_method = NULL;
	_statusCode = 0;
	_request.cleanRequest();
	if ((_request.getRequestState() == Request::firstLine) &&\
		_request.getBufferResidual() > 0)
		return (requestInterraction());
	else
		_state = defaultState;
	return (ok);
}

Client::conditionCode	Client::getNextState(MethodStatus status)
{
	if (status == connectionClosed)
		return (sendingErrorState);
	if (status == error &&\
		(_state == sendingBody || _state == sendindHeader))
		return (sendingErrorState);
	if (status == error)
	{
		if (_state <= analizeHeader)
			createNewMethod();
		return (createHeaders);
	}
	else if (status == ok)
		return (static_cast<Client::conditionCode>\
				(static_cast<int>(_state) + 1));
	return (_state);
}

MethodStatus		Client::analizeHeaders()
{
	MethodStatus	methodStatus = createNewMethod();
	if (methodStatus != ok)
		return (methodStatus);
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
		return (connectionClosed);
	if (_statusCode)
	{
		_method = new MethodGet(_config, _statusCode, _request.getHeadersMap());
		_state = createHeaders;
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
	conditionCode	stateBefore = _state;
	if (_state == defaultState)
		_state = readingHeader;

	if (_state == readingHeader)
		_state = getNextState(_request.getRequestHead());

	if (_state == analizeHeader)
		_state = getNextState(analizeHeaders());

	if (_state == readingBody)
		_state = getNextState(_request.getRequestBody(_method));
	else if (_request.getRequestState() == Request::body)	// cgi case, when we send answer,
		if (_request.getRequestBody(_method) == error)		// before read all request bodey
			_state = sendingErrorState;

	if (_state == manageRequest)
		_state = getNextState(_method->manageRequest(\
					getRequestPath(_request.getURI())));

	if (_state == createHeaders)
		_state = getNextState(_method->createHeader(\
					getRequestPath(_request.getURI())));

	if (stateBefore == _state)
		return (inprogress);
	return (ok);
}

MethodStatus		Client::responseInterraction()
{
	conditionCode	stateBefore = _state;
	if (_state == sendindHeader)
		_state = getNextState(_method->sendHeader(_socket));

	if (_state == sendingBody)
		_state = getNextState(_method->sendBody(_socket));

	if (_state == finalState)
		if (refreshClient() == connectionClosed)
			return (connectionClosed);

	if (stateBefore == _state)
		return (inprogress);
	return (ok);
}

MethodStatus		Client::interract(int newData, int allow2Write)
{
	MethodStatus	returnStatus;
	bool			ready2Send = readyToSend();
	if (newData)
		_state = getNextState(_request.readFromSocket());
	if (isReading())
		returnStatus = requestInterraction();
	if (ready2Send)
		returnStatus = responseInterraction();
	if (_state == sendingErrorState)
		return (error);
	return (returnStatus);
}
