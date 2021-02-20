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
		readRequestBody,
		manageRequest,
		createResponseHeader,
		sendResponseHeader,
		sendResponseBody,
		finalState,
		sendingErrorState,
	}			conditionCode;
	
public:
	int					getClientSocket() const;
	void				setMode(bool mode);
	bool				isReading() const;
	bool				isSending() const;
	MethodStatus		interract();
	int					_requestCounter;

	Client(int socket, s_serv const &config);
	~Client();

private:
	Client();
	conditionCode		_state;
	Request				_request;
	AMethod				*_method;
	int					_statusCode;
	bool				isReadMode;

	t_serv const		&_config;
	int					_socket;

	conditionCode		getNextState(MethodStatus status);
	MethodStatus		requestInterraction();
	MethodStatus		responseInsterraction();
	MethodStatus		refreshClient();
	MethodStatus		createNewMethod();
	MethodStatus		analizeHeaders();
	std::string			getRequestPath(std::string const &uri);

	// MethodStatus		readRequestHeades();
	// MethodStatus		readBodyRequest();
	// MethodStatus		manageRequest();
	// MethodStatus		createHeaders();
	// MethodStatus		sendHeaders();
	// MethodStatus		sendBody();
};
