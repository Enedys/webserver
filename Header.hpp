#pragma once

# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>

#include "Method.hpp"

class Header
{
private:
	std::string const &_path;
	Header();
public:
	Header(std::string const &path);
	~Header();

	void	createGeneralHeaders(stringMap &_headersMap, int &_statusCode);
	void	createEntityHeaders(stringMap &_headersMap, int &_statusCode);
	void	createOptHeaders(stringMap &_headersMap, int &_statusCode);

	void	addAllowHeader(stringMap &_headersMap, int &_statusCode, const t_serv &_config);
	void	addContentLanguageHeader(stringMap &_headersMap, int &_statusCode);
	void	addContentLengthHeader(stringMap &_headersMap, int &_statusCode);
	void	addContentLocationHeader(stringMap &_headersMap, int &_statusCode);
	void	addContentTypeHeader(stringMap &_headersMap, int &_statusCode);

	void	addLastModifiedHeader(stringMap &_headersMap, int &_statusCode);
	void	addLocationHeader(stringMap &_headersMap, int &_statusCode);

	void	addRetryAfterHeader(stringMap &_headersMap, int &_statusCode);
	void	addTransferEncodingHeader(stringMap &_headersMap, int &_statusCode, stringMap const &_headersMapRequest);
	void	addAuthenticateHeader(stringMap &_headersMap, int &_statusCode);

	void	headersToString(stringMap const &headersMap, int const &_statusCode, std::string &output);
};
