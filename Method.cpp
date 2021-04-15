#include "Method.hpp"

AMethod::~AMethod() {}

AMethod::	AMethod(int &status, RequestData &datas) :
	_statusCode(status), data(datas), _bodyType(bodyNotDefined)
{
	_sentBytesTotal = 0;
	_bytesToSend = 0;
	_fd = -1;
	_sendingInProgress = 0;
}

AMethod &AMethod::operator=(AMethod const & src)
{
	if (this == &src)
		return *this;
	_statusCode = src._statusCode;
	cgi = src.cgi;
	_fd = src._fd;
	_type = src._type;
	_bodyType = src._bodyType;
	_output = src._output;
	cgiStatus = src.cgiStatus;
	_sentBytesTotal = src._sentBytesTotal;
	_bytesToSend = src._bytesToSend;
	_remainder = src._remainder;
	_body = src._body;
	_sendingInProgress = src._sendingInProgress;
	return *this;
}

const std::string	AMethod::Methods[AMethod::methodNums] = {
	"GET", "HEAD", "POST", "PUT", "OPTIONS", "DELETE"
};

bool				AMethod::isValidMethod(std::string const &method)
{
	for (int i = 0; i < AMethod::methodNums; i++)
		if (AMethod::Methods[i] == method)
			return (true);
	return (false);
}

MethodStatus		AMethod::sendHeader(int socket)
{
	(void)socket;
	return ok;
}

int					AMethod::getStatusCode() { return _statusCode; }
CGI					&AMethod::getCGI() { return cgi; }
bodyType			&AMethod::getBodyType() {return _bodyType; }
int					&AMethod::getFd() {return _fd; }

void				AMethod::generateErrorPage(std::string &body)
{
	body = "<html>"
		"<style> body {background-color: rgb(247, 247, 247);}"
		"h1 {color: rgb(235, 70, 52);}"
		"e1 {color: rgb(69, 69, 69);} </style>"
		"<body> <h1>ERROR</h1><br><e1>";
	body += size2Dec(_statusCode) + " " + sc[_statusCode];
	body += "</e1></body></html>\n";
}

int					AMethod::generateIdxPage(std::string &body)
{
	DIR				*dir;
	struct dirent	*cur;

	dir = opendir(data.uri.script_name.c_str());
	if (!dir){
		_statusCode = 403;
		return -1;
	}
	body = "<html><head><style> \
					body {background-color: rgb(247, 247, 247);}\
					h1   {color: rgb(235, 70, 52);}\
					td   {color: rgb(75, 8, 23);}\
					a    {color: rgba(255, 99, 71, 1);}\
			</style></head><body>"
			"<h1>Directory index</h1><table style=\"width:80%\">";

	std::string fname(data.uri.request_uri);
	if (fname.at(fname.length() - 1) != '/')
		fname.push_back('/');

	errno = 0;
	while ((cur = readdir(dir)) != NULL){
		if (strcmp(cur->d_name, ".") == 0)
			continue ;

		std::string fullPath(data.uri.script_name);
		if (fullPath.at(fullPath.length() - 1) != '/')
			fullPath.push_back('/');
		fullPath += cur->d_name;

		std::string lastModified = getLastModifiedTime(fullPath);
		struct stat	st;
		stat(fullPath.c_str(), &st);
		size_t size = st.st_size;
		std::string fileSize = size2Dec(size);
		if (S_ISDIR(st.st_mode))
			fileSize = "-";

		body += "<tr><td><a href=\"" + fname + cur->d_name;
		if (S_ISDIR(st.st_mode))
			body += "/";
		body += "\">" + std::string(cur->d_name);
		if (S_ISDIR(st.st_mode))
			body += "/";
		body += "</a></td><td><small>" + lastModified + "</small></td><td><small>" + fileSize + "</small></td></tr><br>";
	}
	body += "</body>\n</html>\n";
	closedir(dir);
	return 0;
}

MethodStatus		AMethod::createHeader()
{
	std::cout << "createHeader: _bodyType: " << _bodyType << std::endl;

	if (_bodyType == bodyIsCGI)
		return ok;
	if ((_type == GET || _type == POST) && _bodyType == bodyIsTextErrorPage)
		generateErrorPage(_body);

	Header		header(data, _statusCode);
	stringMap	hmap;

	header.createGeneralHeaders(hmap);

	if (_type == GET || _type == HEAD || (_type == POST && _bodyType == bodyIsTextErrorPage))//post for debug
		header.addContentLengthHeader(hmap, _body);
	else
		hmap.insert(std::pair<std::string, std::string>("Content-Length", "0"));

	if (_type == GET || _type == HEAD)
	{
		header.addLastModifiedHeader(hmap);
		header.addContentTypeHeader(hmap);
	}
	if (_type == PUT)
	{
		header.addContentLocationHeader(hmap);
		header.addContentTypeHeader(hmap);
	}
	if (_type == OPTION || _statusCode == 405)
		header.addAllowHeader(hmap);

	// header.addRetryAfterHeader(hmap);//503 429
	// header.addTransferEncodingHeader(hmap, hmapRequest);
	header.addAuthenticateHeader(hmap);

	std::string headerStr;
	header.headersToString(hmap, headerStr);
	_body.insert(0, headerStr);

	// std::cout << "_body: " << _body << std::endl;

	return ok;
}

size_t			AMethod::defineRWlimits()
{
	size_t	readBuf = _bs;

	if (_sendingInProgress == 1)
	{
		if (!_remainder.empty()){//что-то не отослалось в send
			readBuf = _bs - _remainder.length();
			_body = _remainder;
		}
		return readBuf;
	}

	if (_bodyType == bodyIsFile)
	{
		struct stat sbuf;
		fstat(_fd, &sbuf);
		_bytesToSend = _body.length() + sbuf.st_size;
		readBuf -= _body.length();
	}
	else
		_bytesToSend = _body.length();

	return readBuf;
}

MethodStatus	AMethod::readFromFileToBuf(size_t limit)
{
	char	buf[_bs + 1];

	memset(buf, 0, _bs);
	size_t res = read(_fd, buf, limit);
	if (res < 0){
		_statusCode = 500;
		close(_fd);
		return error;
	}
	buf[limit] = '\0';
	std::string bufStr(buf, res);
	_body += bufStr;

	return ok;
}

MethodStatus		AMethod::sendBuf(int socket, std::string const & response)
{
	size_t sentBytes = send(socket, response.c_str(), response.length(), MSG_DONTWAIT);
	// if (response.length() > 1)
	// 	std::cout << "Send by OLya: " << response << '\n';
	if (sentBytes < 0 || errno == EMSGSIZE){
		_statusCode = 500;
		close(_fd);
		return error;
	}
	if (sentBytes < response.length()){
		_remainder.assign(response.c_str(), sentBytes, response.length() - sentBytes);
		_sentBytesTotal += sentBytes;
		_sendingInProgress = 1;
		return inprogress;
	};

	_remainder.clear();
	if (_bodyType == bodyIsCGI)
		return _statusCode == ok ? ok : inprogress;

	_sentBytesTotal += sentBytes;
	if (_sentBytesTotal < _bytesToSend){
		_body.clear();
		_sendingInProgress = 1;
		return inprogress;
	}

	// _statusCode = ok;
	_sendingInProgress = 0;
	if (_fd != -1)
		close(_fd);

	return ok;
}

MethodStatus		AMethod::sendResponse(int socket)
{
	size_t readBuf = defineRWlimits();

	// if (_statusCode == 999)
	// {
	// 	_statusCode = 200;
	// }
	if (_bodyType == bodyIsCGI && _remainder.empty())
		if ((_statusCode = cgi.smartOutput(_body)) == inprogress && _body.empty())
			return inprogress;

	if (_bodyType == bodyIsFile)
		if (readFromFileToBuf(readBuf) == error)
			return error;

	std::cout << "_code: " << _statusCode << std::endl;
	MethodStatus status = sendBuf(socket, _body);

	return status;
}
