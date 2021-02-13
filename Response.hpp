#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"

#include "Method.hpp"
#include "MethodError.hpp"
// #include "Request.hpp"
#include "MethodGet.hpp"
#include "MethodHead.hpp"
#include "MethodPut.hpp"
#include "MethodPost.hpp"
#include "MethodOption.hpp"
// #include "TestMethod.hpp"
// class AMethod;

class Response
{
	typedef std::vector<s_loc>::iterator						locIter;
	typedef std::vector<s_loc>::const_iterator					constLocIter;
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
	t_serv const		*_config;
	int					_socket;
	Request	const		*_request;
	AMethod				*_method;
	stageStatus			_stage;
	int					setMethod();
	responseStatus		sendResponse();
	int					setRequest(Request const *request);
	std::string			getPath(std::string const &uri);
	Response();
};
