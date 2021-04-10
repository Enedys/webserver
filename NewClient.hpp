#pragma once
#include "Parser.hpp"
#include "Request.hpp"
#include "Method.hpp"
#include "MethodGet.hpp"
#include "MethodHead.hpp"
#include "MethodPut.hpp"
#include "MethodPost.hpp"
#include "MethodOption.hpp"
#include "MethodDelete.hpp"
#include "RequestData.hpp"
#include "Configurator.hpp"

typedef std::vector<s_loc>::iterator		locIter;
typedef std::vector<s_loc>::const_iterator	constLocIter;

class Client
{
	typedef		enum
	{
		defaultState,
		readingHeader,
		analizeHeader,
		configureIn,
		manageRequest,
		readingBody,
		configureOut,
		createHeaders,
		sendindHeader,
		sendingBody,
		finalState,
		sendingErrorState
	}			conditionCode;

public:
	int					getClientSocket() const;
	int					readyToSend() const;
	bool				isReading() const;
	MethodStatus		interract(int newData, int allow2Write);

	Client(int socket, sockaddr_in adr, t_ext_serv const &config);
	~Client();

private:
	Client();
	Request				_request;
	AMethod				*_method;
	RequestData			procData;

	t_ext_serv const	&_config;
	sockaddr_in			_clientAddr;
	int					_socket;

	conditionCode		_state;
	int					_statusCode;

	conditionCode		getNextState(MethodStatus status);
	MethodStatus		requestInterraction();
	MethodStatus		responseInterraction();
	MethodStatus		refreshClient();
	MethodStatus		createNewMethod();
	MethodStatus		analizeHeaders();
	MethodStatus		configureInput();
	MethodStatus		configureOutput();
};
