#pragma once

# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>

#include "include_resources.hpp"
#include "Parser.hpp"
#include "RequestData.hpp"

extern mapIntStr sc;

class Header
{
private:
	std::string const		&_path;
	std::string const		&_root;
	std::string const		_extension;
	std::string const		_request_uri;
	std::string				_server_name;
	s_loc const				*_location;
	int const				_statusCode;

	Header();
public:
	Header(std::string const &path, std::string const &root, int const statusCode);
	Header(RequestData const &data, int const statusCode);
	~Header();

	void	createGeneralHeaders(stringMap &_headersMap);
	void	createOptHeaders(stringMap &_headersMap);

	void	addAllowHeader(stringMap &_headersMap);
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
};
