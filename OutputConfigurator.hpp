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
	std::string	_outputPage;

	bool		errorOccured() const;
	bool		fileExist(std::string const &f) const;
	bool		isCGI(std::string const &ext) const;
	bool		runCGI(URI const &uri);

	int			openOutputFile(const std::string &f);
	void		configureCorrectOutput();
	void		configureErrorOutput();
	bool		setErrorPage(bsPair const &ep);

	int			inputConfigurator();//(int &flags);////
	bool		findIndexPageInConfig();

public:
	MethodStatus		configure();
	OutputConfigurator(RequestData &data, CGI &cgi, int &statusCode, int const f);
	~OutputConfigurator();

	std::string	const &getOutputPage();
	bodyType	const getBodyType();
	// size_t		const getCGIResponseLength();
};

OutputConfigurator::OutputConfigurator(RequestData &d, CGI &c, int &sc, int const f) :
_data(d), _cgi(c), _statusCode(sc), _bodyType(bodyIsEmpty), inputFlags(f)
{
}

OutputConfigurator::~OutputConfigurator()
{
}

std::string const & OutputConfigurator::getOutputPage()
{
	return this->_outputPage;
};

bodyType	const OutputConfigurator::getBodyType()
{
	return this->_bodyType;
};

// size_t		const OutputConfigurator::getCGIResponseLength()
// {
// 	return this->_cgi.getResponseLength();
// };

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

void	OutputConfigurator::configureCorrectOutput()
{
	cgiStatus cgiStatus = _cgi.getCGIstatus();
	if (cgiStatus == running || cgiStatus == done)
		_bodyType = bodyIsCGI;
	// else if (inputFlags & isAutoindex)
	// 	_bodyType = bodyIsAutoindex;
	else if (_data.getMethod() == "GET" && (_bodyType != bodyIsAutoindex))
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
	return (true);
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

void	OutputConfigurator::configureErrorOutput()
{
	bsPair	configErrorPage = getPageByCode(_data.serv->error_pages, _statusCode);
	if (setErrorPage(configErrorPage) == true)
		return ;
	bsPair	defaultErrorPage = getPageByCode(_data.serv->error_pages, _statusCode);
	if (setErrorPage(defaultErrorPage) == true)
		return ;
	_bodyType = bodyIsTextErrorPage;
}

bool		OutputConfigurator::findIndexPageInConfig()
{
	std::vector<std::string>::iterator it = _data.location->index.begin();
	while (it != _data.location->index.end())
	{
		bsPair indexPage(true, *it);
		if (setErrorPage(indexPage) == true)		//setErrorPage->setPage //start CGI here?
			return true;							//okSuccess;
		it++;
	}
	return false;
};

int			OutputConfigurator::inputConfigurator()	//(int &flags)
{
	struct stat	st;									// if (!fileExist(_data.uri.script_name))	// 	return notFound;
	if (stat(_data.uri.script_name.c_str(), &st) == -1)
		return notFound;

	if (!S_ISDIR(st.st_mode)){						////if (fname.at(fname.length() - 1) != '/')
		_bodyType = bodyIsEmpty;					// or bodyIsFile;
		return 0;
	}

	if (_data.location->autoindex)
		_bodyType = bodyIsAutoindex;				//may updates flags too if needed //when to generate page()
	else											//getting page from config file
	{
		if (findIndexPageInConfig() == true)
			_bodyType = bodyIsFile;
		else
			_bodyType = bodyIsTextErrorPage;
	}
	return 0;
};

MethodStatus	OutputConfigurator::configure()
{
	_statusCode = inputConfigurator();

	if (_cgi.getCGIstatus() == running &&\
		_cgi.getCGIreturnCode() == inprogress)
		return (inprogress);
	if (!errorOccured())
		configureCorrectOutput();
	if (errorOccured())
		configureErrorOutput();

	std::string output;//getOutput();
	if (_bodyType == bodyIsTextErrorPage)
		output = generateErrorPage(_statusCode);
	if (_bodyType == bodyIsAutoindex)
		output = generateIndexPage(_statusCode);

	return (ok);
}
