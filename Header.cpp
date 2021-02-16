#include "Header.hpp"

// * class Header is used to form response headers.
// * Inherits _headersMap field from AMethod to store needed
// for a specific method headers.
// * each function responsible for creating a specific field in _headersMap
// returns 0 upon success and 1 on failure. If 1 function ErrorHeader should be called
// * ErrorHeader deletes all not default fields in _headersMap and adds a field with error info

Header::Header(/* args */)
{
}

Header::~Header()
{
}

// This header must be sent if the server responds with a 405 Method Not Allowed status code
// to indicate which request methods can be used. An empty Allow header indicates that the
// resource allows no request methods, which might occur temporarily for a given resource, for example.
int		addAllowHeader(){
	if (_status == 405)
		_headersMap.insert(std::pair<std::string, std::string>("Allow", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Allow", "GET, HEAD"));
};

