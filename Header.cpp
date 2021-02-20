#include "Header.hpp"

// Header::Header(std::string const &path) : _path(path) {};

Header::~Header(){};

void	Header::headersToString(stringMap const &headersMap, int const &statusCode, std::string *output){
	char *str;//kostylek
	*output += "HTTP/1.1 " + static_cast<std::string>(ft_itoa(statusCode, str, 10)) + " " + "error code explanation" + CRLF;
	for (constMapIter it = headersMap.begin(); it != headersMap.end(); ++it)
		*output += (it->first) + ": " + (it->second) + CRLF;
	*output += CRLF;
	std::cout << "READY_HEADER: " << *output << std::endl;
}

int		Header::createGeneralHeaders(stringMap &_headersMap, int &_statusCode){//common for all methods and errors (Date, Host name(?))

	char			buf1[100];
	struct timeval	tv;
	struct tm		*tm1;

	gettimeofday(&tv, NULL);
	tm1 = gmtime(&tv.tv_sec);
	strftime(buf1, 100, "%a, %d %b %Y %H:%M:%S GMT", tm1);
	std::string date = std::string(buf1);

	_headersMap.insert(std::pair<std::string, std::string>("Server", "nginx/1.2.1"));
	_headersMap.insert(std::pair<std::string, std::string>("Date", date));
	_headersMap.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));//_contentType
	return 0;
};


// This header must be sent if the server responds with a 405 Method Not Allowed status code
// to indicate which request methods can be used. An empty Allow header indicates that the
// resource allows no request methods, which might occur temporarily for a given resource, for example.
int		Header::addAllowHeader(stringMap &_headersMap, int &_statusCode){
	if (_statusCode == 405) {
		_headersMap.insert(std::pair<std::string, std::string>("Allow", ""));
		return 0;
	}
	_headersMap.insert(std::pair<std::string, std::string>("Allow", "GET, HEAD, PUT, POST"));
	return 0;
};

MethodStatus	createErrorHeader(stringMap &_headersMap, int &_statusCode){//not ready
	if (!_headersMap.empty())
		_headersMap.clear();
	char *str;
	_headersMap.insert(std::pair<std::string, std::string>(ft_itoa(_statusCode, str, 10), ""));
	return ok;
};

int		Header::createEntityHeaders(stringMap &_headersMap, int &_statusCode){//specific for statuses and methods
	addContentLanguageHeader(_headersMap, _statusCode);//Entity
	addContentLengthHeader(_headersMap, _statusCode);//Entity //+path
	addContentLocationHeader(_headersMap, _statusCode);//Entity
	addContentTypeHeader(_headersMap, _statusCode);//Entity //+path
	return 0;
};

int		Header::addContentLanguageHeader(stringMap &_headersMap, int &_statusCode){
	// define language of the source or use our default?
	_headersMap.insert(std::pair<std::string, std::string>("Content-Language", "en-US"));//can it be specified in request before?
	return 0;
};

static char* ft_itoa(long num, char* str, int base)
{
	int i = 0;
	bool isNegative = false;

	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}
	if (num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}
	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num/base;
	}
	if (isNegative)
		str[i++] = '-';
	str[i] = '\0';
	int start = 0;
	int end = i - 1;
	while (start < end)	{
		std::swap(*(str + start), *(str + end));
		start++;
		end--;
	}
	return str;
}

int		Header::addContentLengthHeader(stringMap &_headersMap, int &_statusCode){
	// std::string path = "/Users/kwillum/ft_webserver/files/test.file";//

	struct stat stat_buf;
	int rc = stat(_path.c_str(), &stat_buf);
	long fileSize = rc == 0 ? stat_buf.st_size : -1;
	if (fileSize < 0)
		return error;
	char *contentLength;
	contentLength = ft_itoa(fileSize, contentLength, 10);//

	_headersMap.insert(std::pair<std::string, std::string>("Content-Length", contentLength));//can it be specified in request before?
	delete contentLength;//lol
	return 0;
};

int		Header::addContentLocationHeader(stringMap &_headersMap, int &_statusCode){//Entity
//if moved
	return 0;
};

int		Header::addContentTypeHeader(stringMap &_headersMap, int &_statusCode){//Entity
//how to determine type?
	_headersMap.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
	return 0;
};

int		Header::addLastModifiedHeader(stringMap &_headersMap, int &_statusCode){
	char			buf2[100];
	struct stat		stats;
	struct tm		*tm2;
	std::string		_lastModified;

	// std::string path = "/Users/kwillum/ft_webserver/files/test.file";//
	if (stat(_path.c_str(), &stats) == 0) {
		tm2 = gmtime(&stats.st_mtime);
		strftime(buf2, 100, "%a, %d %b %Y %H:%M:%S GMT", tm2);
		_lastModified = std::string(buf2);
	}
	return 0;
}

// При всех перенаправлениях, если метод запроса был не HEAD, то в тело ответа
// следует включить короткое гипертекстовое сообщение с целевым адресом, чтобы
// в случае ошибки пользователь смог сам произвести переход.
// The principal use is to indicate the URL of a resource transmitted
// as the result of content negotiation.
int		Header::addLocationHeader(stringMap &_headersMap, int &_statusCode){
	std::string redirectPath = "/files/test.file";//

	if (_statusCode == 301 || _statusCode == 302 || _statusCode == 303 || _statusCode == 307 || \
	_statusCode == 308 || _statusCode == 201)
	//(() && method != head )
	{
		// if (_statusCode != 201)
			// _body += redirectPath;//if 201 not
		_headersMap.insert(std::pair<std::string, std::string>("Location", redirectPath));
	}
	return 0;
}

// Retry-After: <http-date>
// Retry-After: <delay-seconds>
// When sent with a 503 (Service Unavailable) response, this indicates how long the service is expected to be unavailable.
// When sent with a 429 (Too Many Requests) response, this indicates how long to wait before making a new request.
// When sent with a redirect response, such as 301 (Moved Permanently), this indicates the minimum time that the user agent is asked to wait before issuing the redirected request.
int		Header::addRetryAfterHeader(stringMap &_headersMap, int &_statusCode){

	if (_statusCode == 503 || _statusCode == 429 || _statusCode == 301){
		_headersMap.insert(std::pair<std::string, std::string>("Retry-After", "120"));
	}
	return 0;
}

// specifies the form of encoding used to safely transfer the payload body to the user
// When present on a response to a HEAD request that has no body,
// it indicates the value that would have applied to the corresponding GET message
// Transfer-Encoding: gzip, chunked //compress/deflate/identity
int		Header::addTransferEncodingHeader(stringMap &_headersMap, int &_statusCode){

	_headersMap.insert(std::pair<std::string, std::string>("Transfer-Encoding", "-"));
	return 0;
}

// The WWW-Authenticate header is sent along with a 401 Unauthorized response.
int		Header::addAuthenticateHeader(stringMap &_headersMap, int &_statusCode){

	if (_statusCode == 401)
		_headersMap.insert(std::pair<std::string, std::string>("WWW-Authenticate", "Basic realm=\"Access to the staging site\", charset=\"UTF-8\""));
	return 0;
}

void	Header::setPath(std::string const &path){ this->_path = path; };
