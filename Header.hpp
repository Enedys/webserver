#pragma once

# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>

#include "Method.hpp"

class Header
{
private:
	std::string const	&_path;
	std::string			_errorPage;//body
	int const			_statusCode;//&
	// std::string			&_responseHeaders;//response
	Header();
public:
	Header(std::string const &path, int const statusCode);
	~Header();

	void	createGeneralHeaders(stringMap &_headersMap);
	void	createEntityHeaders(stringMap &_headersMap);
	void	createOptHeaders(stringMap &_headersMap);

	void	addAllowHeader(stringMap &_headersMap, const t_serv &_config);
	void	addContentLanguageHeader(stringMap &_headersMap);
	void	addContentLengthHeader(stringMap &_headersMap, std::string const & body);
	void	addContentLocationHeader(stringMap &_headersMap);
	void	addContentTypeHeader(stringMap &_headersMap);

	void	addLastModifiedHeader(stringMap &_headersMap);
	void	addLocationHeader(stringMap &_headersMap);

	void	addRetryAfterHeader(stringMap &_headersMap);
	void	addTransferEncodingHeader(stringMap &_headersMap, stringMap const &_headersMapRequest);
	void	addAuthenticateHeader(stringMap &_headersMap);

	void	headersToString(stringMap const &headersMap, std::string &output);
	void	generateErrorPage(std::string &body);
	// std::string		&getErrorPage();//
	// std::string		&getResponseHeaders();
};
