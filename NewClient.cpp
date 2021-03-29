#include "NewClient.hpp"

Client::Client(int socket, sockaddr_in adr, t_ext_serv const &config) :
	_socket(socket), _clientAddr(adr), _statusCode(0), _request(socket, _statusCode),\
	_config(config), _method(NULL), _state(defaultState), procData()
{
}

Client::~Client()
{
	delete _method;
	close(_socket);
}

int			 		Client::getClientSocket() const
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

int				Client::readyToSend() const
{
	if (_state == sendingBody || _state == sendindHeader)
		return (1);
	return (0);
}

MethodStatus		Client::refreshClient()
{
	delete _method;
	_method = NULL;
	_statusCode = 0;
	_request.cleanRequest();
	procData.cleanData();
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
		if (_state < analizeHeader)
			return (analizeHeader);
		return (createHeaders);
	}
	else if (status == ok)
		return (static_cast<Client::conditionCode>\
				(static_cast<int>(_state) + 1));
	return (_state);
}

MethodStatus		Client::analizeHeaders()
{
	// if (_statusCode)
	// 	return (createNewMethod());
	procData.setData(&_config, &_request.getHeadersMap(),\
					&_request.getStartLine(), _clientAddr, _statusCode);
	procData.prepareData(_request.getContentLength());
	if (_statusCode == 0 && procData.error_code)
		_statusCode = procData.error_code;
	MethodStatus	methodStatus = createNewMethod();
	return (methodStatus);
}

MethodStatus		Client::createNewMethod()
{
	if (_socket == -1)
		return (connectionClosed);
	if (_statusCode)
	{
		_method = new MethodGet(_statusCode, procData);
		return (error);
	}
	const std::string method = _request.getStartLine().find("method")->second;
	if (method == "GET")
		_method = new MethodGet(_statusCode, procData);
	else if (method == "HEAD")
		_method = new MethodHead(_statusCode, procData);
	else if (method == "OPTIONS")
		_method = new MethodOption(_statusCode, procData);
	else if (method == "PUT")
		_method = new MethodPut(_statusCode, procData);
	else if (method == "POST")
		_method = new MethodPost(_statusCode, procData);
	else if (method == "DELETE")
		_method = new MethodDelete(_statusCode, procData);
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

	if (_state == manageRequest)
		_state = getNextState(_method->manageRequest());

	if (_state == readingBody) 
		_state = getNextState(_request.getRequestBody(_method));
	else if (_request.getRequestState() == Request::body)	// cgi case, when we send answer,
		if (_request.getRequestBody(_method) == error)		//  before read all request bodey
			_state = sendingErrorState;

	if (_state == createHeaders)
		_state = getNextState(_method->createHeader());

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
	bool			ready2Send = (readyToSend() && allow2Write);
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
