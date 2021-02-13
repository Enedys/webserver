# include "MethodGet.hpp"

// MethodGet::MethodGet(t_serv const *config) : _config(config) {
MethodGet::MethodGet(t_serv const *config) : AMethod(config) {
	// check_path_validity?
};//200

MethodGet::~MethodGet(){};

char		*ft_itoa(long int nbr)
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

long		GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

void		mapToString(stringMap const &map, std::string *output){
	for (constMapIter it = map.begin(); it != map.end(); ++it)
		*output += (it->first) + ":" + (it->second) + "\n";
}

MethodStatus	MethodGet::createHeader() {

//"Date":
	char			buf1[100];
	struct timeval	tv;
	struct tm		*tm1;

	gettimeofday(&tv, NULL);
	tm1 = gmtime(&tv.tv_sec);
	strftime(buf1, 100, "%a, %d %b %Y %H:%M:%S GMT", tm1);
	std::string _date = std::string(buf1);

//"Last-Modified":
	char			buf2[100];
	struct stat		stats;
	struct tm		*tm2;
	std::string		_lastModified;

	std::string path = "/Users/bshang/ololo/ws/act/source";//
	if (stat(path.c_str(), &stats) == 0) {
		tm2 = gmtime(&stats.st_mtime);
		strftime(buf2, 100, "%a, %d %b %Y %H:%M:%S GMT", tm2);
		_lastModified = std::string(buf2);
	}

//"Content-Length":
	long fileSize = GetFileSize(path);
	if (fileSize < 0)
		return error;
	char *contentLength = ft_itoa(fileSize);


	_headersMap.insert(std::pair<std::string, std::string>("", "HTTP/1.1 200 OK"));
	_headersMap.insert(std::pair<std::string, std::string>("Server", "nginx/1.2.1"));
	_headersMap.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
	_headersMap.insert(std::pair<std::string, std::string>("Content-Length", contentLength));
	_headersMap.insert(std::pair<std::string, std::string>("Date", _date));

	_headersMap.insert(std::pair<std::string, std::string>("Last-Modified", _lastModified));
	_headersMap.insert(std::pair<std::string, std::string>("Accept-Charsets", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Accept-Language", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Allow", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Authorization", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Content-Language", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Content-Location", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Content-Type", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Host", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Location", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Referer", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Retry-After", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("Transfer-Encoding", "-"));
	_headersMap.insert(std::pair<std::string, std::string>("WWW-Authenticate", "-"));

	delete contentLength;//lol
	return ok;
};

MethodStatus		MethodGet::sendHeader(int socket) {
	std::string headerStr;
	mapToString(_headersMap, &headerStr);
	// std::string &headerStr = mapToString(_headersMap, headerStr);

	if (send(socket, headerStr.c_str(), headerStr.length(), 0) < 0){//reinterpret_cast<const void *>	// if (send(socket, reinterpret_cast<const void *>(header), strlen(header), 0) < 0){//reinterpret
		_status = errorSendHeader;
		return error;
	}

	return ok;
};

MethodStatus		MethodGet::processRequest(std::string &path) {
	struct stat	st;
	if (stat(path.c_str(), &st) == -1){//lstat?
		_status = notFound;
		return error;
	}

	if ((_fd = open(path.c_str(), O_RDONLY | O_NONBLOCK)) <= 0){
		_status = errorOpeningURL;
		return error;
	}

	return ok;
};

MethodStatus		MethodGet::sendBody(int socket) {
// check_socket here too?
	size_t	ret;
	char	buf[BUFSIZE];

	while (ret = read(_fd, buf, BUFSIZE) >= 0){
		int sent = write(socket, buf, ret);
		if (sent == 0)
			break ;
		if (sent < 0){
			_status = errorSocket;
			return error;
		}
	}
	if (ret == -1){
		_status = errorReadingURL;
		return error;
	}
	close(_fd);
	_status = okSuccess;
	return ok;
}
