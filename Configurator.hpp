#pragma once

#include <dirent.h>
#include "RequestData.hpp"
#include "CGI.hpp"
#include "URI.hpp"

typedef std::pair<bool, std::string const &> bsPair;
class Configurator
{
private:
	RequestData		&_data;
	CGI				&_cgi;
	int				&_statusCode;
	int				_fd;
	bodyType		&_bodyType;
	std::string 	const
					*cgi_path;
	bool			errorOccured() const;
	bool			fileExist(std::string const &f) const;
	bool			isAutoindex() const;
	bool			isFile() const;
	bool			CGIisOk() const;
	std::string const
					*isCGI(std::string const &ext) const;

	int				openOutputFile(const std::string &f);
	MethodStatus	runCGI(URI const &uri);
	MethodStatus	configurateCorrectOutput();
	MethodStatus	configurateErrorOutput();
	MethodStatus	setErrorPage(bsPair const &ep);

	bodyType		defineInputType(mode_t const &mode);
	bool			findIndexPageInConfig();
	bool			checkAndSetIndexPage(std::string const &indexPage);

public:
	void			setFd(int &fd) const;
	Configurator(RequestData &data, CGI &cgi, int &statusCode, bodyType &bodyType);
	~Configurator();
	MethodStatus		configurate();
	MethodStatus		inputConfigurator();

	bodyType		getBodyType() const;
	void			updateBodyType(bodyType &newType);
};
