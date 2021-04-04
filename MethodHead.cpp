#include "MethodHead.hpp"
#include "Header.hpp"

MethodHead::~MethodHead() {};
MethodStatus 	MethodHead::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };
MethodStatus	MethodHead::sendResponse(int socket) { return (ok); };
MethodStatus	MethodHead::sendHeader(int socket) { return (ok); };

MethodStatus	MethodHead::manageRequest()
{
	if (_bodyType == bodyIsAutoindex)
	{
		_statusCode = 403;
		return ok;
		// _bodyType = bodyNotDefined;///// bodyIsError
	}
	_statusCode = 200;
	return ok;
};

MethodStatus	MethodHead::createHeader()
{
	if (_bodyType == bodyIsCGI){
		// _statusCode = cgi.init(data);//return 200
		return ok;
	}
	// if (_bodyType == bodyIsTextErrorPage)
	// 	generateErrorPage(_body);

	Header		header(data.uri.script_name, data.location->root, _statusCode);
	stringMap	hmap;

	std::cout << "////\t\t HEAD METHOD, statusCode: " << _statusCode << std::endl;

	header.createGeneralHeaders(hmap);
	header.addContentLengthHeader(hmap, _body);

	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206))
		header.createEntityHeaders(hmap);

	if (_statusCode == 405)
		header.addAllowHeader(hmap, *data.location);
	header.addLocationHeader(hmap);
	header.addRetryAfterHeader(hmap);
	// header.addTransferEncodingHeader(hmap, _hmapRequest);
	header.addAuthenticateHeader(hmap);

	std::string headerStr;
	header.headersToString(hmap, headerStr);
	_body.insert(0, headerStr);

	std::cout << "_body: " << _body << std::endl;


	return ok;
};
