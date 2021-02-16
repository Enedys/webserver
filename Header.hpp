#ifndef HEADER_HPP
# define HEADER_HPP

#include "Method.hpp"

// * class Header is used to form response headers.
// * Inherits _headersMap field from AMethod to store needed
// for a specific method headers.
// * each function responsible for creating a specific field in _headersMap
// returns 0 upon success and 1 on failure. If 1 function ErrorHeader should be called
// * ErrorHeader deletes all not default fields in _headersMap and adds a field with error info
class Header : public AMethod
{
private:
	int		_status;//if header creation was ok

public:
	Header();
	~Header();

	int		createDefaultHeader();//common for all methods and errors (Date, Host name(?))
	int		addAllowHeader();
	int		addDateHeader();
};

Header::Header(/* args */)
{
}

Header::~Header()
{
}

int		addAllowHeader(){
	
};

#endif
