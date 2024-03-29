#include "MethodGet.hpp"
#include "Header.hpp"
#include "Configurator.hpp"

MethodGet::~MethodGet(){}

MethodStatus	MethodGet::processBody(const std::string &requestBody, MethodStatus bodyStatus)
{
	if (_bodyType == bodyIsCGI)
	{
		if (_statusCode >= 400)
			return bodyStatus;
		cgi.input(requestBody, bodyStatus);
		return (bodyStatus);
	}
	(void)requestBody;
	(void)bodyStatus;
	return ok;
}

MethodStatus	MethodGet::manageRequest()
{
	if (!data.location->getAvailable)
		_statusCode = 405;

	else if (_bodyType == bodyIsAutoindex && (generateIdxPage(_body) < 0))
		_statusCode = 404;

	else if (_bodyType == bodyIsCGI)
	{
		constMapIter cgi_iter = data.location->cgi.find(data.uri.extension);
		if (cgi_iter ==  data.location->cgi.end())
		{
			_statusCode = 405;
			return error;
		}
		data.setCGIbin((*cgi_iter).second);
		// data.cgi_bin = (*cgi_iter).second;
		// if (!fileExist(_data.cgi_bin))
		// 	return error;
		data.createCGIEnv();
		if (!data.cgi_conf)
			return error;
		_statusCode = cgi.init(data);
	}
	return ok;
}
