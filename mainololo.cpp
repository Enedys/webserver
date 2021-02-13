# include <iostream>
# include <map>
// # include <pair>
# include <sys/stat.h>
# include <fstream>
# include <time.h>

# include <stdlib.h>
# include <sys/time.h>
# include <fcntl.h>//write nonblock

# include "unistd.h"

# define BUFSIZE 1024
# define error 1

int		sendBody(int socket, std::string &sourcepath) {
// check_socket here too?
	size_t	ret;
	char	buf[BUFSIZE];
	int		_fd;

	if ((_fd = open(sourcepath.c_str(), O_RDONLY | O_NONBLOCK)) <= 0){
		// _status = errorOpeningURL;
		return error;
	}
	std::cout << "\n_fd:\t" << _fd << std::endl;

	while ((ret = read(_fd, buf, BUFSIZE)) >= 0){// >= 0 //
		std::cout << "\nread:\t" << ret << std::endl;
		std::cout << "buf:\t"  << buf << std::endl;
		int sent = write(socket, buf, ret);//, O_NONBLOCK);
		if (sent == 0)//
			break ;
		if (sent < 0){
			// _status = errorSocket;
			return error;
		}
	}
	if (ret == -1){
		// _status = errorReadingURL;
		return error;
	}


	close(_fd);
	// _status = 200;
	return 0;
}

long GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

long FdGetFileSize(int fd)
{
	struct stat stat_buf;
	int rc = fstat(fd, &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

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

// std::string		*mapToString(std::map<std::string, std::string>  &map){
void	mapToString(std::map<std::string, std::string>  &map, std::string *output){
	for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it)
		*output += (it->first) + ": " + (it->second) + "\n";
	// return output;
}

int main(){
	// std::string socketpath = "/Users/bshang/ololo/ws/act/target";
	std::string path = "/Users/bshang/ololo/ws/act/source";

	// int socket = open(socketpath.c_str(), O_RDWR);
	// std::cout << "socket:\t" << socket;
	// sendBody(socket, path);



	char			buffer[100];
	struct timeval	tv;
	struct tm		*tm1;

	gettimeofday(&tv, NULL);
	tm1 = gmtime(&tv.tv_sec);
	strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm1);
	std::string _date = std::string(buffer);


	memset(&buffer, 0, 100);
	struct stat		stats;
	struct tm		*tm2;
	std::string		_lastModified;

	if (stat(path.c_str(), &stats) == 0) {
		tm2 = gmtime(&stats.st_mtime);
		strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm2);
		_lastModified = std::string(buffer);
	}

	// std::cout << _date << "\n" << _lastModified << std::endl;

	std::map<std::string, std::string> header;

	// insert elements in random order
	header.insert(std::pair<std::string, std::string>("", "HTTP/1.1 200 OK"));
	header.insert(std::pair<std::string, std::string>("Server", "nginx/1.2.1"));
	header.insert(std::pair<std::string, std::string>("Content-Type", "text/html"));
	char *contentLength = ft_itoa(GetFileSize(path));
	header.insert(std::pair<std::string, std::string>("Content-Length", contentLength));
	delete contentLength;
	header.insert(std::pair<std::string, std::string>("Date", _date));
	header.insert(std::pair<std::string, std::string>("Last-Modified", _lastModified));
	header.insert(std::pair<std::string, std::string>("Accept-Charsets", "-"));
	header.insert(std::pair<std::string, std::string>("Accept-Language", "-"));
	header.insert(std::pair<std::string, std::string>("Allow", "-"));
	header.insert(std::pair<std::string, std::string>("Authorization", "-"));
	header.insert(std::pair<std::string, std::string>("Content-Language", "-"));
	header.insert(std::pair<std::string, std::string>("Content-Location", "-"));
	header.insert(std::pair<std::string, std::string>("Content-Type", "-"));
	header.insert(std::pair<std::string, std::string>("Host", "-"));
	header.insert(std::pair<std::string, std::string>("Location", "-"));
	header.insert(std::pair<std::string, std::string>("Referer", "-"));
	header.insert(std::pair<std::string, std::string>("Retry-After", "-"));
	header.insert(std::pair<std::string, std::string>("Transfer-Encoding", "-"));
	header.insert(std::pair<std::string, std::string>("WWW-Authenticate", "-"));
	// header["Date"] = _date;
	// header["Last-Modified"] = _lastModified;
	typedef std::map<std::string, std::string>::const_iterator	constMapIter;
	for (constMapIter it=header.begin(); it!=header.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';

	std::string headerStr;
	mapToString(header, &headerStr);

	std::cout << "\n\nstringified header: \n" << headerStr << std::endl;
	// std::map<std::string, std::string> map;
	// map["hello"] = "world";
	// std::map<std::string, std::string>::iterator it;
	// it = map.begin();
	// std::cout << "Begin: " << it->first.length() << std::endl;
	// it = map.end();
	// std::cout << "End: " << it->first.length() << std::endl;
}
