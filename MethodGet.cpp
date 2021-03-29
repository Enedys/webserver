#include "MethodGet.hpp"
#include "Header.hpp"

MethodGet::~MethodGet(){ delete _header; };
MethodStatus		MethodGet::sendResponse(int socket) { return ok; };
MethodStatus		MethodGet::sendHeader(int socket) { return ok; };

void			MethodGet::generateIdxPage(){

	DIR				*dir;
	struct dirent	*cur;

	dir = opendir(data.uri.script_name.c_str());
	if (!dir){//EACCES
		_statusCode = errorOpeningURL;
		return ;
	}
	_body = "<html><head><style> \
					body {background-color: rgb(252, 243, 233);}\
					h1   {color: lightseagreen;}\
					td   {color: rgb(75, 8, 23);}\
					a    {color: rgba(255, 99, 71, 1);}\
			</style></head><body>"
			"<h1>Directory index</h1><table style=\"width:80%\">";

	std::string fname(data.uri.request_uri);
	if (fname.at(fname.length() - 1) != '/')
		fname.push_back('/');

	errno = 0;
	while ((cur = readdir(dir)) != NULL){//) && errno == 0){
		if (strcmp(cur->d_name, ".") == 0)
			continue ;

		std::string fullPath(data.uri.script_name);
		if (fullPath.at(fullPath.length() - 1) != '/')
			fullPath.push_back('/');
		fullPath += cur->d_name;
		struct stat	st;
		char buf[100];
		stat(fullPath.c_str(), &st);
		struct tm *tm2 = gmtime(&st.st_mtime);
		strftime(buf, 100, "%d-%b-%Y %H:%M\n", tm2);
		std::string lastModified(buf);
		size_t size = st.st_size;
		std::string fileSize = size2Dec(size);
		if (S_ISDIR(st.st_mode))
			fileSize = "-";

		_body += "<tr><td><a href=\"" + fname + cur->d_name;
		std::cout << "_body: " << _body << std::endl;
		if (S_ISDIR(st.st_mode))
			_body += "/";
		_body += "\">" + std::string(cur->d_name);
		if (S_ISDIR(st.st_mode))
			_body += "/";
		_body += "</a></td><td><small>" + lastModified + "</small></td><td><small>" + fileSize + "</small></td></tr><br>";
	}
	_body += "</body>\n</html>\n";
	closedir(dir);
}

MethodStatus	MethodGet::processBody(const std::string &requestBody, MethodStatus bodyStatus) { return (ok); };

// POST || .php .py (cgi in config) || GET HEAD + no autoindex + IS_DIR + stat(index.php) in config - manageRequest
//is error and error_page is set ad cgi extension - create header

//always allowed. but others...
MethodStatus	MethodGet::manageRequest()
{

	if (_statusCode != 0)
		return ok;

	// если файл сжи

	struct stat	st;
	_statusCode = okSuccess;
	if (stat(data.uri.script_name.c_str(), &st) == -1){// && errno == ENOENT)
		_statusCode = notFound;
		return error;
	}

	if (S_ISDIR(st.st_mode)){
		if (data.location->autoindex)
			generateIdxPage();
		else {
			_statusCode = errorOpeningURL;//403 Forbidden
			// check index pages.
			//   - взять первый файл если сжи то
			//   - если есть ехе файлы
		}
	}

	// else if (S_ISREG(st.st_mode) && (_fd = open(data.uri.script_name.c_str(), O_RDONLY | O_NONBLOCK)) < 0){// else S_ISFIFO S_ISLNK /// O_DIRECTORY
	else if ((_fd = open(data.uri.script_name.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
		_statusCode = errorOpeningURL;
	return ok;
};

MethodStatus	MethodGet::createHeader()
{
	_header = new Header(data.uri.script_name, data.location->root, _statusCode);

	std::cout << "\n////\tGET METHOD, statusCode: " << _statusCode << std::endl;

	_header->createGeneralHeaders(_headersMap);

	if (_statusCode < 200 || _statusCode > 206)
		_header->generateErrorPage(_body, data.serv->error_pages);
	// else if (_body.length() == 0)///
	_header->addContentLengthHeader(_headersMap, _body);//for GET//body for auto+error//if not dir!

	if (_statusCode == 0 || (_statusCode >= 200 && _statusCode <= 206))
		_header->createEntityHeaders(_headersMap);
	if (_statusCode == 405)
		_header->addAllowHeader(_headersMap, *data.location);
	_header->addLocationHeader(_headersMap);//if redirect
	_header->addRetryAfterHeader(_headersMap);//503 429
	// _header->addTransferEncodingHeader(_headersMap, _headersMapRequest);
	_header->addAuthenticateHeader(_headersMap);

	return ok;
};

MethodStatus		MethodGet::sendBody(int socket)
{
	std::string	response;
	size_t		readBytes;
	size_t		sentBytes;
	char		buf[_bs + 1];
	size_t		readBuf = _bs;

	memset(buf, 0, _bs);
	if (_statusCode != okSendingInProgress){
		_header->headersToString(_headersMap, response);
		size_t headersize = response.length();

		if (!_body.empty()){//only for autoindex & error page
			response += _body;
			_bytesToSend = response.length();
		}
		else {
			struct stat sbuf;
			fstat(_fd, &sbuf);
			_bytesToSend = headersize + sbuf.st_size;
			readBuf -= headersize;
		}
	}
	if (!_remainder.empty()){//only if not a full response was sent (by send)
		readBuf = _bs - _remainder.length();
		response = _remainder;
	}
	if (_body.empty()){//not listing, regular file
		readBytes = read(_fd, buf, readBuf);
		// std::cout << "_________readBytes: " << readBytes << "\n" << std::endl;
		if (readBytes < 0){
			_statusCode = errorReadingURL;
			close(_fd);
			return error;//what happens after, above?
		}
		buf[readBuf] = '\0';
		std::string bufStr(buf, readBytes);
		response += bufStr;
	}

	sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	if (sentBytes < 0 || errno == EMSGSIZE){
		_statusCode = errorSendingResponse;
		close(_fd);
		return error;
	}
	if (sentBytes < response.length()){
		_remainder.assign(response.c_str(), sentBytes, response.length() - sentBytes);
		_sentBytesTotal += sentBytes;
		_statusCode = okSendingInProgress;
		return inprogress;
	};
	// _remainder.clear();//
	_sentBytesTotal += sentBytes;
	if (_sentBytesTotal < _bytesToSend){
		_statusCode = okSendingInProgress;
		return inprogress;
	}

	close(_fd);
	_body.clear();
	return ok;
}
