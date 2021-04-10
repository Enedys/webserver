#include "MethodGet.hpp"
#include "Header.hpp"
#include "Configurator.hpp"

MethodGet::~MethodGet(){};
MethodStatus		MethodGet::sendHeader(int socket) { return ok; };

MethodStatus	MethodGet::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };

// POST || .php .py (cgi in config) || GET HEAD + no autoindex + IS_DIR + stat(index.php) in config - manageRequest
//is error and error_page is set ad cgi extension - create header

MethodStatus	MethodGet::manageRequest()
{
	// if (_statusCode != 0)//по старой логике - надо ли оставить
	// 	return ok;
	std::cout << "_bodyType manageRequest: " << _bodyType << std::endl;

	if (!data.location->getAvailable)
		_statusCode = methodNotAllowed;

	else if (_bodyType == bodyIsAutoindex && (generateIdxPage(_body) < 0))
		_statusCode = 404;

	else if (_bodyType == bodyIsCGI)
	{
		constMapIter cgi_iter = data.location->cgi.find(data.uri.extension);
		if (cgi_iter ==  data.location->cgi.cend())
			return (error);
		data.setCGIbin((*cgi_iter).second);
		// data.cgi_bin = (*cgi_iter).second;
		// if (!fileExist(_data.cgi_bin))
		// 	return (error);
		data.createCGIEnv();
		if (!data.cgi_conf)
			return (error);
		_statusCode = cgi.init(data);//return 200
	}
	return ok;//
};

	// 0 bodyNotDefined,
	// 1 bodyIsEmpty,
	// 2 bodyIsAutoindex,		// text
	// 3 bodyIsTextErrorPage,	// text
	// 4 bodyIsFile,			// file: regularFile, indexFile, errorFile
	// 5 bodyIsCGI				// cgi

// MethodStatus	MethodGet::createHeader()//createResponse()
// {
// 	std::cout << "_bodyType createHeader: " << _bodyType << std::endl;
// 	if (_bodyType == bodyIsCGI)
// 		return ok;

// 	if (_bodyType == bodyIsTextErrorPage)//bodyNotDefined)//bodyIsTextErrorPage)////->сказать Дане, надо bodyIsTextErrorPage)
// 		generateErrorPage(_body);

// 	Header		header(data.uri.script_name, data.location->root, _statusCode);
// 	stringMap	hmap;
// 	std::cout << "\n////\tGET METHOD, statusCode: " << _statusCode << std::endl;

// 	header.createGeneralHeaders(hmap);
// 	header.addContentLengthHeader(hmap, _body);//for GET//body for auto+error//if not dir!

// 	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206)){
// 		header.addLastModifiedHeader(hmap);
// 		header.addContentLocationHeader(hmap);
// 		if (_bodyType != bodyIsCGI && _bodyType != bodyIsEmpty)
// 			header.addContentTypeHeader(hmap, data.uri.extension);
// 	}

// 	if (_statusCode == 405)
// 		header.addAllowHeader(hmap, *data.location);

// 	// header.addLocationHeader(hmap, *data.location, data.uri.request_uri);//if redirect
// 	// header.addRetryAfterHeader(hmap);//503 429
// 	// header.addTransferEncodingHeader(hmap, hmapRequest);
// 	// header.addAuthenticateHeader(hmap);

// 	std::string headerStr;
// 	header.headersToString(hmap, headerStr);
// 	_body.insert(0, headerStr);

// 	return ok;
// };
