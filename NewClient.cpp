#include "NewClient.hpp"

Client::Client(int socket, sockaddr_in adr, t_ext_serv const &config) :
	_request(socket, _statusCode), _method(NULL), procData(config, adr, _statusCode),\
	_config(config), _clientAddr(adr), _socket(socket), _state(defaultState), _statusCode(0)
{
}

Client::~Client()
{
	delete _method;
	_method = NULL;
	procData.cleanData();
	_request.cleanRequest();
	close(_socket);
}

int			 		Client::getClientSocket() const
{
	return _socket;
}

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
	int	finalStatus = _statusCode;
	delete _method;
	_method = NULL;
	_statusCode = 0;
	_request.cleanRequest();
	procData.cleanData();
	_state = defaultState;
	if (finalStatus >= 400)
		return (connectionClosed);
	else if (_request.getBufferResidual() > 0)
		return (requestInterraction());
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
		return (configureOut);
	}
	else if (status == ok)
		return (static_cast<Client::conditionCode>\
				(static_cast<int>(_state) + 1));
	return (_state);
}

MethodStatus		Client::analizeHeaders()
{
	procData.setData(&_request.getHeadersMap(),\
						&_request.getStartLine(),\
							_request.getContentLength());
	procData.prepareData();
	MethodStatus	methodStatus = createNewMethod();
	if (procData.location)
		_request.setBodyLimit(procData.location->bodySizeLimit);
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

MethodStatus		Client::configureOutput()
{
	Configurator	outConf(procData, _method->getCGI(),\
						_statusCode, _method->getBodyType());
	if (outConf.configurate() == inprogress)
		return (inprogress);
	if (_method->getBodyType() == bodyIsFile)
		outConf.setFd(_method->getFd());
	return (ok);
}

MethodStatus		Client::configureInput()
{
	Configurator	outConf(procData, _method->getCGI(),\
						_statusCode, _method->getBodyType());
	outConf.inputConfigurator();
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

	if (_state == configureIn)
		_state = getNextState(configureInput());

	if (_state == manageRequest)
		_state = getNextState(_method->manageRequest());

	if (_state == readingBody)
		_state = getNextState(_request.getRequestBody(_method));
//	else if (_request.getRequestState() == Request::body)	// cgi case, when we send answer,
//		if (_request.getRequestBody(_method) == error)		//  before read all request bodey
//			_state = sendingErrorState;

	if (_state == configureOut)
		_state = getNextState(configureOutput());

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
		_state = getNextState(_method->sendResponse(_socket));

	if (_state == finalState)
		if (refreshClient() == connectionClosed)
			_state = sendingErrorState;

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
