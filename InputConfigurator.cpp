#include "Configurator.hpp"

bodyType const	Configurator::getBodyType()
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
	uri.setTranslatedPath(_data.serv->root);
	if (fileExist(uri.script_name) == false)
		return false;
	if (isCGI(uri.extension))						// Danya
	{
		_bodyType = bodyIsCGI;
		return true;
	}
	_fd = open(uri.script_name.c_str(), O_RDONLY | O_NONBLOCK);
	if (_fd < 0)
		return false;
	_data.uri = uri;
	_bodyType = bodyIsFile;

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

	if (findIndexPageInConfig() == true)
		if (isCGI(_data.uri.extension))
			return bodyIsCGI;

	_statusCode = 403;

	return bodyNotDefined;//здесь было: bodyIsError - что вместо него
}

MethodStatus	Configurator::inputConfigurator()
{
	struct stat	st;
	if (stat(_data.uri.script_name.c_str(), &st) == -1)
	{
		_statusCode = notFound;
		return error;
	}

	_bodyType = defineInputType(st.st_mode);

	// if (_bodyType == bodyIsCGI)// а что c delete? // думаю, что cgi должен все же запускать метод
	// 	_statusCode = _cgi.init();
	std::cout << "inputConfigurator: _bodyType: " << _bodyType << std::endl;

	return ok;
}
