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


class Response
{
	public:
	typedef std::vector<s_loc>::iterator						locIter;
	typedef std::vector<s_loc>::const_iterator				constLocIter;

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
	int					setMethod();
	responseStatus		sendResponse();
	int					setRequest(Request const *request);
	std::string			getPath(std::string const &uri);
private:
	t_serv const		*_config;
	int					_socket;
	Request	const		*_request;
	AMethod				*_method;
	stageStatus			_stage;
	Response();
};
