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

class Header : public AMethod
{
// protected://
// 	int		_status;//if header creation was ok

public:
	Header();
	~Header();

	int		createGeneralHeaders();//common for all responses
	int		createEntityHeaders();
	MethodStatus	createErrorHeader();//int
	// int		createOptHeaders();//specific for statuses and methods

	int		addAllowHeader();//405 Method Not Allowed//no req methods if empty
	int		addContentLanguageHeader();//Entity
	int		addContentLengthHeader();//Entity //+path
	int		addContentLocationHeader();//Entity
	int		addContentTypeHeader();//Entity //+path

	int		addLastModifiedHeader();//+path
	int		addLocationHeader();//+path

	int		addRetryAfterHeader();//if status
	int		addTransferEncodingHeader();
	int		addAuthenticateHeader();



	MethodStatus	sendHeader(int socket);//
	void	mapToString(stringMap const &startLine, stringMap const &headersMap, std::string *output);//
};

#endif
