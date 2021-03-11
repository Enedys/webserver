#include "MethodOption.hpp"

// MethodOption::MethodOption(t_serv const &config, int &code, stringMap const &headers) : AMethod(config, code, headers) {};
MethodOption::~MethodOption() {};
MethodStatus	MethodOption::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return ok; };
MethodStatus	MethodOption::sendResponse(int socket) { return ok; }
MethodStatus	MethodOption::manageRequest()
{
	if (!data.serv->locs[0].optionsAvailable)
		_statusCode = methodNotAllowed;
	else
		_statusCode = okSuccess;
	return ok;
};

MethodStatus	MethodOption::createHeader()
{
	// _header = new Header(data.uri.script_name, _statusCode);
	_header = new Header(data.uri.script_name, data.location->root, _statusCode);
	std::cout << "////\t\t OPTIONS METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap);
	if (_statusCode < 200 || _statusCode > 206)
		_header->generateErrorPage(_body, data.serv->error_pages);
	_headersMap.insert(std::pair<std::string, std::string>("Content-Length", "0"));
	_header->addAllowHeader(_headersMap, *data.serv);

	return ok;
};

MethodStatus	MethodOption::sendHeader(int socket){ return ok; };

MethodStatus	MethodOption::sendBody(int socket)
{//send _body (if error?)
	std::string	response;
	size_t		sentBytes;

	if (_remainder.length())
		response = _remainder;
	if (_statusCode != okSendingInProgress)
		_header->headersToString(_headersMap, response);

	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || errno == EMSGSIZE){
		_statusCode = errorSendingResponse;
		return error;
	}
	if (sentBytes < response.length()){
		_remainder.assign(response.c_str(), sentBytes, response.length() - sentBytes);
		_statusCode = okSendingInProgress;
		return inprogress;
	};
	_remainder.clear();
	return ok;
};
