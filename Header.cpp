#include "Header.hpp"

Header::Header(std::string const &path, std::string const &root, int const statusCode)
	: _path(path), _root(root), _statusCode(statusCode) {};

Header::~Header(){ };

mapIntStr statusCodes;
static struct statusCodesInit
{
	statusCodesInit(){
		statusCodes[200] = "OK";
		statusCodes[201] = "Created";
		statusCodes[202] = "Accepted";
		statusCodes[400] = "Bad Request";
		statusCodes[401] = "Unauthorized";
		statusCodes[403] = "Forbidden";//when autoindex is off and request is a folder
		statusCodes[404] = "Not Found";
		statusCodes[405] = "Method Not Allowed";
		statusCodes[406] = "Not Acceptable";
		statusCodes[408] = "Request Timeout";
		statusCodes[411] = "Length Required";
		statusCodes[414] = "URI Too Long";
		statusCodes[415] = "Unsupported Media Type";
		statusCodes[418] = "I'm a teapot";
		statusCodes[421] = "Misdirected Request";
		statusCodes[422] = "Unprocessable Entity";
		statusCodes[429] = "Too Many Requests";
		statusCodes[431] = "Request Header Fields Too Large";
		statusCodes[449] = "Retry With";
		statusCodes[499] = "Client Closed Request";
		statusCodes[500] = "Internal Server Error";
		statusCodes[501] = "Not Implemented";
		statusCodes[503] = "Service Unavailable";//перегрузка сервера
		statusCodes[505] = "HTTP Version Not Supported";
	}
} statusCodesInit;

mapIntStr defaultErrorPages;
static struct defaultErrorPagesInit
{
	defaultErrorPagesInit(){
		defaultErrorPages[404] = "/errors/404-error.jpg";
	}
} defaultErrorPagesInit;

void	Header::headersToString(stringMap const &headersMap, std::string &output)
{
	std::string statusCodeStr = size2Dec(_statusCode);
	output += "HTTP/1.1 " + statusCodeStr + " " + statusCodes[_statusCode] + CRLF;
	for (constMapIter it = headersMap.begin(); it != headersMap.end(); ++it)
		output += (it->first) + ": " + (it->second) + CRLF;
	output += CRLF;
}

int		Header::generateImagePage(std::string &body, std::string &errorurl)
{
	size_t len = _root.length();
	if (_root.back() == '/')
		len--;
	std::string path(_root, 0, len);
	if (*errorurl.begin() != '/')
		path += '/';
	path += errorurl;
	int fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
		return -1;
	struct stat st;
	int rc = stat(path.c_str(), &st);
	if (rc < 0 || !S_ISREG(st.st_mode))
		return -1;
	char buf[4096 * 24];
	int res = read(fd, buf, st.st_size);
	std::string bufStr(buf, st.st_size);
	close(fd);
	body = bufStr;
	return 0;
};

void	Header::generateErrorPage(std::string &body, mapIntStr const &error_pages)
{
	std::string errorurl;
	constMapIntStrIter cit;
	mapIntStrIter it;
	if ((cit = error_pages.find(_statusCode)) != error_pages.end()){
		std::cout << "custom" << std::endl;
		errorurl = cit->second;
		if (generateImagePage(body, errorurl) == 0)
			return ;
	}
	if ((it = defaultErrorPages.find(_statusCode)) != defaultErrorPages.end()){
		std::cout << "default" << std::endl;
		errorurl = it->second;
		if (generateImagePage(body, errorurl) == 0)
			return ;
	}
	body = "<html>"
		"<style> body {background-color: rgb(252, 243, 233);}"
		"h1 {color: rgb(200, 0, 0);}"
		"e1 {color: rgb(100, 0, 0);} </style>"
		"<body> <h1>ERROR</h1><br><e1>";
	body += size2Dec(_statusCode) + " " + statusCodes[_statusCode];//add explanation
	body += "</e1></body></html>\n";
};

void	Header::createGeneralHeaders(stringMap &headersMap)
{
	char			buf1[100];
	struct timeval	tv;
	struct tm		*tm1;

	gettimeofday(&tv, NULL);
	tm1 = gmtime(&tv.tv_sec);
	strftime(buf1, 100, "%a, %d %b %Y %H:%M:%S GMT", tm1);
	std::string date = std::string(buf1);
	headersMap.insert(std::pair<std::string, std::string>("Server", "Shabillum/1.0.7"));
	headersMap.insert(std::pair<std::string, std::string>("Date", date));
};

void	Header::createEntityHeaders(stringMap &headersMap)
{//specific for statuses and methods
	// addContentLanguageHeader(headersMap);
	addContentLocationHeader(headersMap);//redir?
	// addContentTypeHeader(headersMap);
	addLastModifiedHeader(headersMap);//if modified
};

void	Header::addContentLanguageHeader(stringMap &headersMap)
{
	headersMap.insert(std::pair<std::string, std::string>("Content-Language", "en-US"));//can it be specified in request before?
};

void	Header::addContentLengthHeader(stringMap &headersMap, std::string const & body)
{
	size_t bodySize;
	struct stat stat_buf;

	if (body.length())
		bodySize = body.length();
	else {
		int rc = stat(_path.c_str(), &stat_buf);
		if (rc == -1 || S_ISDIR(stat_buf.st_mode))
			return ;
		bodySize = stat_buf.st_size;
	}
	std::string contentLength = size2Dec(bodySize);
	headersMap.insert(std::pair<std::string, std::string>("Content-Length", contentLength));//can it be specified in request before?
};

void	Header::addContentLocationHeader(stringMap &_headersMap)
{//if moved
// Content-Location indicates the direct URL to use to access the resource, without further
// content negotiation in the future. Location is a header associated with the response,
//  while Content-Location is associated with the data returned
};

void	Header::addContentTypeHeader(stringMap &_headersMap)
{//type from cgi ot rhm/conf
	_headersMap.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
};

void	Header::addLastModifiedHeader(stringMap &headersMap)
{
	char			buf2[100];
	struct stat		stats;
	struct tm		*tm2;
	std::string		lastModified;

	if (stat(_path.c_str(), &stats) == 0) {
		tm2 = gmtime(&stats.st_mtime);
		strftime(buf2, 100, "%a, %d %b %Y %H:%M:%S GMT", tm2);
		lastModified = std::string(buf2);
	}
	headersMap.insert(std::pair<std::string, std::string>("Last-Modified", lastModified));
}

// This header must be sent if the server responds with a 405 Method Not Allowed status code
// to indicate which request methods can be used. An empty Allow header indicates that the
// resource allows no request methods, which might occur temporarily for a given resource, for example.
void	Header::addAllowHeader(stringMap &_headersMap, const t_serv &_config)
{
	std::cout << "////\t\taddAllowHeader" << _statusCode << std::endl;
	// int statusCode1 = const_cast<int&>(statusCode) = 405;
	std::string allowedMethods;// = "";
	if (_config.locs[0].getAvailable)
		allowedMethods += "GET, ";
	if (_config.locs[0].postAvailable)
		allowedMethods += "POST, ";
	if (_config.locs[0].headAvailable)
		allowedMethods += "HEAD, ";
	if (_config.locs[0].putAvailable)
		allowedMethods += "PUT, ";
	if (_config.locs[0].optionsAvailable)
		allowedMethods += "OPTIONS, ";
	if (_config.locs[0].deleteAvailable)
		allowedMethods += "DELETE";
	_headersMap.insert(std::pair<std::string, std::string>("Allow", allowedMethods));
};

// При всех перенаправлениях, если метод запроса был не HEAD, то в тело ответа
// следует включить короткое гипертекстовое сообщение с целевым адресом, чтобы
// в случае ошибки пользователь смог сам произвести переход.
// The principal use is to indicate the URL of a resource transmitted
// as the result of content negotiation.
void	Header::addLocationHeader(stringMap &_headersMap)
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
void	Header::addRetryAfterHeader(stringMap &_headersMap)
{
	if (_statusCode == 503 || _statusCode == 429)// || _statusCode == 301){
		_headersMap.insert(std::pair<std::string, std::string>("Retry-After", "120"));
}

// specifies the form of encoding used to safely transfer the payload body to the user
// When present on a response to a HEAD request that has no body,
// it indicates the value that would have applied to the corresponding GET message
// Transfer-Encoding: gzip, chunked //compress/deflate/identity
void	Header::addTransferEncodingHeader(stringMap &_headersMap, stringMap const &_headersMapRequest)
{
	constMapIter	itc = _headersMapRequest.begin();
	while (itc != _headersMapRequest.end())
	{
		std::cout << "Header: " << itc->first << ", Value: " << itc->second << std::endl;
		itc++;
	}

	constMapIter it = _headersMapRequest.find("transfer-encoding");
	std::cout << "addTransferEncodingHeader" << std::endl;
	if (it != _headersMapRequest.end())
	{
		std::cout << "inside" << std::endl;
		_headersMap.insert(std::pair<std::string, std::string>("transfer-encoding", it->second));
		std::cout << it->second << std::endl;
	}
}

void	Header::addAuthenticateHeader(stringMap &_headersMap)
{
	if (_statusCode == 401)
		_headersMap.insert(std::pair<std::string, std::string>("WWW-Authenticate", "Basic realm=\"Access to the staging site\", charset=\"UTF-8\""));
}
