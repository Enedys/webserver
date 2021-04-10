#include "MethodHead.hpp"
#include "Header.hpp"

MethodHead::~MethodHead() {};
MethodStatus 	MethodHead::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodHead::sendHeader(int socket) { return (ok); };

MethodStatus	MethodHead::manageRequest()
{
	std::cout << "_bodyType manageRequest: " << _bodyType << std::endl;
	_bodyType = bodyIsEmpty;

	if (!data.location->headAvailable)
		_statusCode = methodNotAllowed;

	else if (_bodyType == bodyIsAutoindex)//isCGI
		_statusCode = 403;

	return ok;
};

// MethodStatus	MethodHead::createHeader()
// {
// 	Header		header(data.uri.script_name, data.location->root, _statusCode);
// 	stringMap	hmap;

// 	std::cout << "////\t\t HEAD METHOD, statusCode: " << _statusCode << std::endl;
// 	std::cout << "_bodyType createHeader: " << _bodyType << std::endl;

// 	header.createGeneralHeaders(hmap);
// 	if (_statusCode < 400){
// 		header.addContentLengthHeader(hmap, _body);
// 		header.addContentTypeHeader(hmap, data.uri.extension);//
// 	}
// 	// else
// 	// 	hmap.insert(std::pair<std::string, std::string>("Content-Length", "0"));//can it be specified in request before?

// 	if (_statusCode == 405)
// 		header.addAllowHeader(hmap, *data.location);

// 	// if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206))
// 	// 	header.createEntityHeaders(hmap);

// 	// header.addLocationHeader(hmap);
// 	// header.addRetryAfterHeader(hmap);
// 	// header.addTransferEncodingHeader(hmap, _hmapRequest);
// 	// header.addAuthenticateHeader(hmap);

// 	std::string headerStr;
// 	header.headersToString(hmap, headerStr);
// 	_body.insert(0, headerStr);

// 	return ok;
// };
