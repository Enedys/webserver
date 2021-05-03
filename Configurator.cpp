#include "Configurator.hpp"

Configurator::Configurator(RequestData &d, CGI &c, int &statusCode, bodyType &bodyType) :
_data(d), _cgi(c), _statusCode(statusCode), _bodyType(bodyType)
{
	cgi_path = NULL;
	_fd = -1;
}

void	Configurator::setFd(int &fd) const
{
	if (_bodyType != bodyIsFile)
		return ;
	if (fd == _fd)
		return ;
	if (fd != -1)
		close(fd);
	fd = _fd;
}

Configurator::~Configurator()
{
}

bool	Configurator::errorOccured() const
{
	if (_statusCode < 400)
		return (false);
	return (true);
}

bool		Configurator::CGIisOk() const
{
	cgiStatus cgiStatus = _cgi.getStatus();
	if (cgiStatus == running || cgiStatus == done)
		return (true);
	return (false);
}

std::string const
		*Configurator::isCGI(std::string const &ext) const
{
	if (!_data.serv)
		return (NULL);
	if (_data.location)
	{
		constMapIter cgi_iter = _data.location->cgi.find(ext);
		if (cgi_iter ==  _data.location->cgi.end())
			return (NULL);
		return (&((*cgi_iter).second));
	}
	else
	{
		constMapIter cgi_iter = _data.serv->cgi.find(ext);
		if (cgi_iter ==  _data.serv->cgi.end())
			return (NULL);
		return (&((*cgi_iter).second));
	}
}

bool	Configurator::fileExist(std::string const &f) const
{
	struct stat	st;
	if (stat(f.c_str(), &st) == -1)
		return (false);
	return (true);
}

int		Configurator::openOutputFile(std::string const &f)
{
	if (!fileExist(f))
		return 404;
	if ((_fd = open(f.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
		return 403;
	return 0;
}

bool	Configurator::isFile() const
{		//maybe just check that _bodyType is a file?
	std::string const	*method_name = _data.getMethod();
	if (!method_name)
		return (false);
	if (*method_name == "GET" ||\
		*method_name == "HEAD")
		return (true);
	return (false);
}

MethodStatus
		Configurator::configurateCorrectOutput()
{
	if (CGIisOk())
		_bodyType = bodyIsCGI;
	else if (_bodyType == bodyIsAutoindex)
		;
	else if (isFile())
	{
		int	open_status = openOutputFile(_data.uri.script_name);
		if (open_status != 0) //open error occur
		{
			_statusCode = open_status;
			return (ok);
		}
		if (*(_data.getMethod()) == "HEAD")
		{
			close(_fd);
			_bodyType = bodyIsEmpty;
		}
		else
			_bodyType = bodyIsFile;
	}
	else
		_bodyType = bodyIsEmpty;
	return (ok);
}

bsPair	getPageByCode(mapIntStr const &map, int code)
{
	constMapIntStrIter	errorPageIter = map.find(code);
	if (errorPageIter != map.end())
		return bsPair(true, (*errorPageIter).second);
	return bsPair(false, std::string(""));
}

MethodStatus
		Configurator::runCGI(URI const &uri)
{
	(void)uri;
	_data.cgi_bin = *cgi_path;
	if (!fileExist(_data.cgi_bin))
		return (error);
	_data.createCGIEnv();
	if (!_data.cgi_conf)
		return (error);
	_cgi.init(_data);
	return (inprogress);
}

MethodStatus
		Configurator::setErrorPage(bsPair const &ep)
{
	bool	found = ep.first;
	if (!found)
		return (error);
	URI		uri;
	uri.procUri(ep.second);
	//uri.setTranslatedPath(_data.serv->root);
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

mapIntStr defaultErrorPages;
static struct defaultErrorPagesInit
{
	defaultErrorPagesInit(){
		defaultErrorPages[403] = "/errors/403.png";
		defaultErrorPages[404] = "/errors/404.png";
		defaultErrorPages[405] = "/errors/405.png";
	}
} defaultErrorPagesInit;

MethodStatus
		Configurator::configurateErrorOutput()
{
	if (_bodyType == bodyIsEmpty)
		return (ok);

	if (CGIisOk())
	{
		_bodyType = bodyIsCGI;
		return (ok);
	}
	bsPair	configErrorPage = getPageByCode(_data.serv->error_pages, _statusCode);
	MethodStatus	_status = setErrorPage(configErrorPage);
	if (_status != error)
		return (_status);
	bsPair	defaultErrorPage = getPageByCode(defaultErrorPages, _statusCode); // here default pages need to find
	_status = setErrorPage(defaultErrorPage);
	if (_status != error)
		return (_status);
	_bodyType = bodyIsTextErrorPage;
	return (ok);
}

MethodStatus
		Configurator::configurate()
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
