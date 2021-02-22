#pragma once
#include "Parser.hpp"
#include "Request.hpp"
#include "Method.hpp"
#include "MethodGet.hpp"
#include "MethodHead.hpp"
#include "MethodPut.hpp"
#include "MethodPost.hpp"
#include "MethodOption.hpp"

typedef std::vector<s_loc>::iterator		locIter;
typedef std::vector<s_loc>::const_iterator	constLocIter;

class Client
{
	typedef		enum
	{
		defaultState,
		readingHeader,
		analizeHeader,
		readingBody,
		manageRequest,
		createHeaders,
		sendindHeader,
		sendingBody,
		finalState,
		sendingErrorState,
	}			conditionCode;
	
public:
	int					getClientSocket() const;
	bool				isReading() const;
	bool				readyToSend() const;
	MethodStatus		interract(int newData, int allow2Write);


	Client(int socket, s_serv const &config);
	~Client();

private:
	Client();
	conditionCode		_state;
	Request				_request;
	AMethod				*_method;
	int					_statusCode;

	t_serv const		&_config;
	int					_socket;

	conditionCode		getNextState(MethodStatus status);
	MethodStatus		requestInterraction();
	MethodStatus		responseInterraction();
	MethodStatus		refreshClient();
	MethodStatus		createNewMethod();
	MethodStatus		analizeHeaders();
	std::string			getRequestPath(std::string const &uri);
};
