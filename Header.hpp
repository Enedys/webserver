#ifndef HEADER_HPP
# define HEADER_HPP

# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>

#include "Method.hpp"

// * class Header is used to form response headers.
// * Inherits _headersMap field from AMethod to store needed
// for a specific method headers.
// * each function responsible for creating a specific field in _headersMap
// returns 0 upon success and 1 on failure. If 1 function ErrorHeader should be called
// * ErrorHeader deletes all not default fields in _headersMap and adds a field with error info

// need to share response status of the exec processes to add some headers

// class Header : public AMethod
class Header
{
// protected://
// 	int		_status;//if header creation was ok

public:
	Header();
	~Header();

	int	createGeneralHeaders(stringMap &_headersMap, int &_statusCode);//common for all responses
	int	createEntityHeaders(stringMap &_headersMap, int &_statusCode);
	MethodStatus createErrorHeader(stringMap &_headersMap, int &_statusCode);//int
	// int	createOptHeaders(stringMap &_headersMap, int &_statusCode);//specific for statuses and methods

	int	addAllowHeader(stringMap &_headersMap, int &_statusCode);//405 Method Not Allowed//no req methods if empty
	int	addContentLanguageHeader(stringMap &_headersMap, int &_statusCode);//Entity
	int	addContentLengthHeader(stringMap &_headersMap, int &_statusCode);//Entity //+path
	int	addContentLocationHeader(stringMap &_headersMap, int &_statusCode);//Entity
	int	addContentTypeHeader(stringMap &_headersMap, int &_statusCode);//Entity //+path

	int	addLastModifiedHeader(stringMap &_headersMap, int &_statusCode);//+path
	int	addLocationHeader(stringMap &_headersMap, int &_statusCode);//+path

	int	addRetryAfterHeader(stringMap &_headersMap, int &_statusCode);//if status
	int	addTransferEncodingHeader(stringMap &_headersMap, int &_statusCode);
	int	addAuthenticateHeader(stringMap &_headersMap, int &_statusCode);


	// MethodStatus	sendHeader(int socket);//
	void headersToString(stringMap const &headersMap, int const &_statusCode, std::string *output);
};

#endif
