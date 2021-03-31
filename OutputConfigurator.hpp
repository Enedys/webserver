#pragma once

#include "RequestData.hpp"
#include "CGI.hpp"
#include <dirent.h>
#include "URI.hpp"

typedef std::pair<bool, std::string const &> bsPair;
class OutputConfigurator
{
private:
	RequestData		&_data;
	CGI				&_cgi;
	int				&_statusCode;
	int				_fd;
	bodyType		_bodyType;
	std::string 	const
					*cgi_path;
	int const		inputFlags;
	bool			errorOccured() const;
	bool			fileExist(std::string const &f) const;
	bool			isAutoindex() const;
	bool			CGIisOk() const;
	std::string const
					*isCGI(std::string const &ext) const;

	int				openOutputFile(const std::string &f);
	MethodStatus	runCGI(URI const &uri);
	MethodStatus	configurateCorrectOutput();
	MethodStatus	configurateErrorOutput();
	MethodStatus	setErrorPage(bsPair const &ep);

public:
	MethodStatus		configurate();
	OutputConfigurator(RequestData &data, CGI &cgi, int &statusCode, int const f);
	~OutputConfigurator();
};

OutputConfigurator::OutputConfigurator(RequestData &d, CGI &c, int &sc, int const f) :
_data(d), _cgi(c), _statusCode(sc), _bodyType(bodyIsEmpty), inputFlags(f)
{
	cgi_path = NULL;
}

OutputConfigurator::~OutputConfigurator()
{
}

bool	OutputConfigurator::errorOccured() const
{
	if (_statusCode < 400)
		return (false);
	return (true);
}

bool		OutputConfigurator::CGIisOk() const
{
	cgiStatus cgiStatus = _cgi.getStatus();
	if (cgiStatus == running || cgiStatus == done)
		return (true);
	return (false);
}

std::string const
		*OutputConfigurator::isCGI(std::string const &ext) const
{
	if (!_data.serv)
		return (NULL);
	if (_data.location)
	{
		constMapIter cgi_iter = _data.location->cgi.find(ext);
		if (cgi_iter ==  _data.location->cgi.cend())
			return (NULL);
		return (&((*cgi_iter).second));
	}
	else
	{
		constMapIter cgi_iter = _data.serv->cgi.find(ext);
		if (cgi_iter ==  _data.serv->cgi.cend())
			return (NULL);
		return (&((*cgi_iter).second));
	}
}

bool	OutputConfigurator::fileExist(std::string const &f) const
{
	struct stat	st;
	if (stat(f.c_str(), &st) == -1)
		return (false);
	return (true);
}

int		OutputConfigurator::openOutputFile(std::string const &f)
{
	if (!fileExist(f))
		return 404;
	if ((_fd = open(f.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
		return 403;
	return 0;
}

MethodStatus
		OutputConfigurator::configurateCorrectOutput()
{
	if (CGIisOk())
		_bodyType = bodyIsCGI;
	else if (_bodyType == bodyIsAutoindex)
		;
	else if (_data.getMethod() == "GET" ||\
			_data.getMethod() == "HEAD")
	{
		int	open_status = openOutputFile(_data.uri.script_name);
		if (open_status == 0)
			_bodyType = bodyIsFile;
		else
			_statusCode = open_status;		//error occur
	}
	else
		_bodyType = bodyIsEmpty;
}

bsPair	getPageByCode(mapIntStr const &map, int code)
{
	constMapIntStrIter	errorPageIter = map.find(code);
	if (errorPageIter != map.cend())
		return bsPair(true, (*errorPageIter).second);
	return bsPair(false, std::string(""));
}

MethodStatus
		OutputConfigurator::runCGI(URI const &uri)
{
	_data.cgi_bin = *cgi_path;
	_data.createCGIEnv();
	if (!_data.cgi_conf)
		return (error);
	// here i start CGI with updated data
	// smth like CGI.init(_data.cgi_conf)
	// if cgi return error - ok :(
	// call cgi.input("", ok) immediatly!
	return (inprogress);
}

MethodStatus
		OutputConfigurator::setErrorPage(bsPair const &ep)
{
	bool	found = ep.first;
	if (!found)
		return (error);
	URI		uri;
	uri.procUri(ep.second);
	uri.setTranslatedPath(_data.serv->root);
	_data.uri = uri;
	if (!fileExist(uri.script_name))
		return (error);
	cgi_path = isCGI(uri.extension);
	if (cgi_path)
		return (runCGI(uri));
	int	open_status = openOutputFile(uri.script_name);
	if (open_status != 0)
		return (error);
	_bodyType = bodyIsFile;
	return (ok);
}

MethodStatus
		OutputConfigurator::configurateErrorOutput()
{
	if (CGIisOk())
	{
		_bodyType = bodyIsCGI;
		return (ok);
	}
	bsPair	configErrorPage = getPageByCode(_data.serv->error_pages, _statusCode);
	MethodStatus	_status = setErrorPage(configErrorPage); 
	if (_status != error)
		return (_status);
	bsPair	defaultErrorPage = getPageByCode(_data.serv->error_pages, _statusCode); // here default pages need to find
	_status = setErrorPage(defaultErrorPage); 
	if (_statusCode != error)
		return (_status);
	_bodyType = bodyIsTextErrorPage;
	return (ok);
}

MethodStatus
		OutputConfigurator::configurate()
{
	if (_cgi.getStatus() == running &&\
		_cgi.httpStatus == 0)
		return (inprogress);
	MethodStatus	return_status;
	if (!errorOccured())
		return_status = configurateCorrectOutput();
	if (errorOccured())
		return_status = configurateErrorOutput();
	return (return_status);
} 
