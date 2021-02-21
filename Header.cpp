#include "Header.hpp"

Header::Header(std::string const &path) : _path(path) {};

Header::~Header(){ delete this; };

static char		*ft_ltoa(long int nbr)
{
	int		len = 0;
	int		sign = 1;
	char	*str;
	long int nbrcpy = nbr;

	while (nbrcpy <= -10 || nbrcpy >= 10) {
		nbrcpy /= 10;
		len++;
	}
	if (nbrcpy < 0)
		len++;
	if (!(str = (char*)malloc(sizeof(*str) * (len + 2))))
		return (NULL);
	if (nbr < 0){
		nbr *= -1;
		sign *= (-1);
	}
	if (sign < 0)
		str[0] = '-';
	str[len + 1] = '\0';
	while (len >= 0){
		str[len] = nbr % 10 + '0';
		nbr = nbr / 10;
		len--;
		if (len == 0 && sign < 0)
			break ;
	}
	return (str);
}

std::map<int, std::string> respStatusCodes;
struct respStatusCodesInit
{
	respStatusCodesInit()
	{
		respStatusCodes[200] = "OK";
		respStatusCodes[201] = "Created";
		respStatusCodes[202] = "Accepted";
		respStatusCodes[400] = "Bad Request";
		respStatusCodes[401] = "Unauthorized";
		respStatusCodes[403] = "Forbidden";
		respStatusCodes[404] = "Not Found";
		respStatusCodes[405] = "Method Not Allowed";
		respStatusCodes[414] = "URI Too Long";
		respStatusCodes[429] = "Too Many Requests";
		respStatusCodes[503] = "Service Unavailable";//перегрузка сервера
		respStatusCodes[505] = "HTTP Version Not Supported";
	}
} respStatusCodesInit;

void	Header::headersToString(stringMap const &headersMap, int const &statusCode, std::string *output)
{
	*output += "HTTP/1.1 " + static_cast<std::string>(ft_ltoa(statusCode)) + " " + respStatusCodes[statusCode] + CRLF;
	for (constMapIter it = headersMap.begin(); it != headersMap.end(); ++it)
		*output += (it->first) + ": " + (it->second) + CRLF;
	*output += CRLF;
}

void	Header::createGeneralHeaders(stringMap &_headersMap, int &_statusCode)
{
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
};

void	Header::createEntityHeaders(stringMap &_headersMap, int &_statusCode)
{//specific for statuses and methods
	addContentLanguageHeader(_headersMap, _statusCode);
	addContentLengthHeader(_headersMap, _statusCode);
	addContentLocationHeader(_headersMap, _statusCode);
	addContentTypeHeader(_headersMap, _statusCode);
	addLastModifiedHeader(_headersMap, _statusCode);//if modified
};

void	Header::addContentLanguageHeader(stringMap &_headersMap, int &_statusCode){
	_headersMap.insert(std::pair<std::string, std::string>("Content-Language", "en-US"));//can it be specified in request before?
};

void	Header::addContentLengthHeader(stringMap &_headersMap, int &_statusCode)
{
	struct stat stat_buf;
	int rc = stat(_path.c_str(), &stat_buf);
	long fileSize = rc == 0 ? stat_buf.st_size : -1;
	if (fileSize < 0)
		return ;
	char *contentLength;
	contentLength = ft_ltoa(fileSize);//
	std::string contentLengthStr(contentLength);
	delete contentLength;
	_headersMap.insert(std::pair<std::string, std::string>("Content-Length", contentLengthStr));//can it be specified in request before?
};

void	Header::addContentLocationHeader(stringMap &_headersMap, int &_statusCode)
{//if moved
// Content-Location indicates the direct URL to use to access the resource, without further
// content negotiation in the future. Location is a header associated with the response,
//  while Content-Location is associated with the data returned
};

void	Header::addContentTypeHeader(stringMap &_headersMap, int &_statusCode)
{//how to determine type?
	_headersMap.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
};

void	Header::addLastModifiedHeader(stringMap &_headersMap, int &_statusCode)
{
	char			buf2[100];
	struct stat		stats;
	struct tm		*tm2;
	std::string		_lastModified;

	if (stat(_path.c_str(), &stats) == 0) {
		tm2 = gmtime(&stats.st_mtime);
		strftime(buf2, 100, "%a, %d %b %Y %H:%M:%S GMT", tm2);
		_lastModified = std::string(buf2);
	}
	_headersMap.insert(std::pair<std::string, std::string>("Last-Modified", _lastModified));
}

// This header must be sent if the server responds with a 405 Method Not Allowed status code
// to indicate which request methods can be used. An empty Allow header indicates that the
// resource allows no request methods, which might occur temporarily for a given resource, for example.
void	Header::addAllowHeader(stringMap &_headersMap, int &_statusCode, const t_serv &_config)
{
	// int statusCode1 = const_cast<int&>(_statusCode) = 405;
	if (_statusCode != 405)
		return ;
	std::string allowedMethods = "";
	if (_config.locs[0].getAvailable)
		allowedMethods += "GET, ";
	if (_config.locs[0].postAvailable)
		allowedMethods += "POST, ";
	if (_config.locs[0].headAvailable)
		allowedMethods += "HEAD, ";
	if (_config.locs[0].putAvailable)
		allowedMethods += "PUT";
	_headersMap.insert(std::pair<std::string, std::string>("Allow", allowedMethods));
};

// При всех перенаправлениях, если метод запроса был не HEAD, то в тело ответа
// следует включить короткое гипертекстовое сообщение с целевым адресом, чтобы
// в случае ошибки пользователь смог сам произвести переход.
// The principal use is to indicate the URL of a resource transmitted
// as the result of content negotiation.
void	Header::addLocationHeader(stringMap &_headersMap, int &_statusCode)
{
	std::string redirectPath = "/files/test.file";//
	if (_statusCode == 301 || _statusCode == 302 || _statusCode == 303 || _statusCode == 307 || \
	_statusCode == 308 || _statusCode == 201)
	//(() && method != head )
	{
		// if (_statusCode != 201)
			// _body += redirectPath;//if 201 not
		_headersMap.insert(std::pair<std::string, std::string>("Location", redirectPath));
	}
}

// Retry-After: <http-date>
// Retry-After: <delay-seconds>
// When sent with a 503 (Service Unavailable) response, this indicates how long the service is expected to be unavailable.
// When sent with a 429 (Too Many Requests) response, this indicates how long to wait before making a new request.
// When sent with a redirect response, such as 301 (Moved Permanently), this indicates the minimum time that the user agent is asked to wait before issuing the redirected request.
void	Header::addRetryAfterHeader(stringMap &_headersMap, int &_statusCode)
{
	if (_statusCode == 503 || _statusCode == 429)// || _statusCode == 301){
		_headersMap.insert(std::pair<std::string, std::string>("Retry-After", "120"));
}

// specifies the form of encoding used to safely transfer the payload body to the user
// When present on a response to a HEAD request that has no body,
// it indicates the value that would have applied to the corresponding GET message
// Transfer-Encoding: gzip, chunked //compress/deflate/identity
void	Header::addTransferEncodingHeader(stringMap &_headersMap, int &_statusCode)
{
	// from Method->_headersMapRequest
	// _headersMap.insert(std::pair<std::string, std::string>("Transfer-Encoding", "-"));
}

void	Header::addAuthenticateHeader(stringMap &_headersMap, int &_statusCode)
{
	if (_statusCode == 401)
		_headersMap.insert(std::pair<std::string, std::string>("WWW-Authenticate", "Basic realm=\"Access to the staging site\", charset=\"UTF-8\""));
}
