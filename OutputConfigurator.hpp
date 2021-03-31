#pragma once

#include <dirent.h>
#include "RequestData.hpp"
#include "CGI.hpp"
#include "URI.hpp"

typedef std::pair<bool, std::string const &> bsPair;
class OutputConfigurator
{
private:
	RequestData	&_data;
	CGI			&_cgi;
	int			&_statusCode;
	int			_fd;
	bodyType	_bodyType;
	int const	inputFlags;

	bool		errorOccured() const;
	bool		fileExist(std::string const &f) const;
	bool		isCGI(std::string const &ext) const;
	bool		runCGI(URI const &uri);

	int			openOutputFile(const std::string &f);
	void		configurateCorrectOutput();
	void		configurateErrorOutput();
	bool		setErrorPage(bsPair const &ep);

	bodyType	defineInputType(mode_t const &mode);
	bool		findIndexPageInConfig();
	bool		checkAndSetIndexPage(std::string const &indexPage);

public:
	OutputConfigurator(RequestData &data, CGI &cgi, int &statusCode, int const f);
	~OutputConfigurator();
	MethodStatus		configurate();
	MethodStatus		inputConfigurator();

	bodyType const	getBodyType();
	void			updateBodyType(bodyType &newType);
};

OutputConfigurator::OutputConfigurator(RequestData &d, CGI &c, int &sc, int const f) :
_data(d), _cgi(c), _statusCode(sc), _bodyType(bodyIsEmpty), inputFlags(f)
{
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

bool	OutputConfigurator::isCGI(std::string const &ext) const
{
	constMapIter cgi_iter = _data.serv->cgi.find(ext);
	if (cgi_iter !=  _data.serv->cgi.cend())
		return (true);
	return (false);
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

void	OutputConfigurator::configurateCorrectOutput()
{
	cgiStatus cgiStatus = _cgi.getCGIstatus();
	if (cgiStatus == running || cgiStatus == done)
		_bodyType = bodyIsCGI;
	else if (inputFlags & isAutoindex)
		_bodyType = bodyIsAutoindex;
	else if (_data.getMethod() == "GET")
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

bool	OutputConfigurator::runCGI(URI const &uri)
{
	_data.cgi_bin = (*(_data.serv->cgi.find(uri.extension))).second;
	_data.createCGIEnv(0);
	if (!_data.cgi_conf)
		return (false);
	// here i start CGI with updated data
	// smth like CGI.init(_data.cgi_conf)
	// if cgi return error - ok :(
	// call cgi.input("", ok) immediatly!
	return (true);//не забыть установить bodyType = iscgi
}

bool	OutputConfigurator::setErrorPage(bsPair const &ep)
{
	URI		uri;
	bool	found = ep.first;
	if (!found)
		return (false);
	uri.procUri(ep.second);
	uri.setTranslatedPath(_data.serv->root);
	_data.uri = uri;
	if (!fileExist(uri.script_name))
		return (false);
	if (isCGI(uri.extension))
		return (runCGI(uri));
	int	open_status = openOutputFile(uri.script_name);
	if (open_status != 0)
		return (false);
	_bodyType = bodyIsFile;
	return (true);
}

void	OutputConfigurator::configurateErrorOutput()
{
	bsPair	configErrorPage = getPageByCode(_data.serv->error_pages, _statusCode);
	if (setErrorPage(configErrorPage) == true)
		return ;
	bsPair	defaultErrorPage = getPageByCode(_data.serv->error_pages, _statusCode);
	if (setErrorPage(defaultErrorPage) == true)
		return ;
	_bodyType = bodyIsTextErrorPage;
}

MethodStatus	OutputConfigurator::configurate()
{
	if (_cgi.getCGIstatus() == running &&\
		_cgi.getCGIreturnCode() == inprogress)
		return (inprogress);
	if (!errorOccured())
		configurateCorrectOutput();
	if (errorOccured())
		configurateErrorOutput();
	return (ok);
}
