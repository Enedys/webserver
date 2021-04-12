#pragma once

# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>

#include "RequestData.hpp"
#include "include_resources.hpp"

extern mapIntStr sc;

class Header
{
private:
	// RequestData const		&_data;
	std::string const		&_path;
	std::string const		&_root;
	std::string const		_extension;
	s_loc const				*_location;
	int const				_statusCode;
	static const stringMap	_mimeMap;

	Header();
public:
	Header(std::string const &path, std::string const &root, int const statusCode);
	Header(RequestData const &data, int const statusCode);
	~Header();

	void	createGeneralHeaders(stringMap &_headersMap);
	void	createEntityHeaders(stringMap &_headersMap);
	void	createOptHeaders(stringMap &_headersMap);

	void	addAllowHeader(stringMap &_headersMap);
	void	addContentLanguageHeader(stringMap &_headersMap);
	void	addContentLengthHeader(stringMap &_headersMap, std::string const & body);
	// void	addContentLocationHeader(stringMap &_headersMap, const s_loc &location, const std::string &request_uri);
	void	addContentLocationHeader(stringMap &_headersMap);
	void	addContentTypeHeader(stringMap &_headersMap);

	void	addLastModifiedHeader(stringMap &_headersMap);
	void	addLocationHeader(stringMap &_headersMap);
	void	addRetryAfterHeader(stringMap &_headersMap);
	void	addTransferEncodingHeader(stringMap &_headersMap, stringMap const &_headersMapRequest);
	void	addAuthenticateHeader(stringMap &_headersMap);

	void	headersToString(stringMap const &headersMap, std::string &output);
};
