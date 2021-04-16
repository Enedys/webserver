#include "Configurator.hpp"

bodyType		Configurator::getBodyType() const
{
	return this->_bodyType;
};

void			Configurator::updateBodyType(bodyType &newType)
{
	this->_bodyType = newType;
};

bool			Configurator::checkAndSetIndexPage(std::string const &indexPage)
{
	URI uri;

	uri.procUri(indexPage);
	uri.setTranslatedPath(_data.uri.script_name);
	if (fileExist(uri.script_name) == false)
		return false;
	if (isCGI(uri.extension))
	{
		_data.uri.extension = uri.extension;
		_data.uri.script_name = uri.script_name;
		_bodyType = bodyIsCGI;
		return true;
	}
//	_fd = open(uri.script_name.c_str(), O_RDONLY | O_NONBLOCK);
//	if (_fd < 0)
//		return false;
	_bodyType = bodyIsFile;
	_data.uri = uri;

	return true;
}

bool			Configurator::findIndexPageInConfig()
{
	std::vector<std::string>::const_iterator it = _data.location->index.begin();
	while (it != _data.location->index.end())
	{
		if (checkAndSetIndexPage(*it) == true)
			return true;
		it++;
	}
	return false;
};

bodyType		Configurator::defineInputType(mode_t const &mode)
{
	if (!S_ISDIR(mode))
		return isCGI(_data.uri.extension) ? bodyIsCGI : bodyIsFile;
	// in methods that do not have body in response, bodyType will be updated
	// to bodyIsEmpty or to whatever if needed - it's the responsibility of Method

	if (_data.location->autoindex)
		return bodyIsAutoindex;

	if (*(_data.getMethod()) == "DELETE")
		return bodyIsEmpty;
	if (*(_data.getMethod()) == "HEAD" || *(_data.getMethod()) == "PUT"){
		_statusCode = 403;
		return bodyIsEmpty;
	}
	if (findIndexPageInConfig() == true)
		return isCGI(_data.uri.extension) ? bodyIsCGI : bodyIsFile;

	_statusCode = 404;
	return bodyNotDefined;
}

MethodStatus	Configurator::inputConfigurator()
{
	struct stat	st;
	if (stat(_data.uri.script_name.c_str(), &st) == -1)
	{
		_statusCode = 404;
		return error;
	}

	_bodyType = defineInputType(st.st_mode);

	std::cout << "inputConfigurator: _bodyType: " << _bodyType << std::endl;

	return ok;
}
