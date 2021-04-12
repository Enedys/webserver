#include "MethodGet.hpp"
#include "Header.hpp"
#include "Configurator.hpp"

MethodGet::~MethodGet(){};
MethodStatus		MethodGet::sendHeader(int socket) { return ok; };

MethodStatus	MethodGet::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };

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
			return error;
		data.setCGIbin((*cgi_iter).second);
		// data.cgi_bin = (*cgi_iter).second;
		// if (!fileExist(_data.cgi_bin))
		// 	return error;
		data.createCGIEnv();
		if (!data.cgi_conf)
			return error;
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
