#include "Method.hpp"

AMethod::~AMethod() {};

// AMethod::AMethod(t_serv const &config, int &status, stringMap const &headers) :
// 	_config(config), _statusCode(status), _headersMapRequest(headers) {};
AMethod::	AMethod(int &status, RequestData const &datas) :
	_statusCode(status), data(datas), _bodyType(bodyNotDefined)
{};

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
	char resp[] = "HTTP/1.1 200 OK\r\n"
	"Server: nginx/1.2.1\r\n"
	"Date: Sat, 08 Mar 2014 22:53:46 GMT\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 58\r\n"
	"Last-Modified: Sat, 08 Mar 2014 22:53:30 GMT\r\n\r\n";
	char bodya[] = "<html>"
	"<head>"
	"</head>"
 	"<body>"
   	"<h1>Hello World<h1>"
 	"</body>"
	"</html>";
	std::string	letter(resp);
	letter = letter + bodya;
	send(socket, letter.c_str(), letter.length(), MSG_DONTWAIT);
	return (ok);
}

int					AMethod::getStatusCode() { return _statusCode; };
CGI					&AMethod::getCGI() { return cgi; };
bodyType			&AMethod::getBodyType() {return _bodyType; };
int					&AMethod::getFd() {return _fd; };

std::string			&AMethod::generateErrorPage()
{
	std::string body;
	body = "<html>"
		"<style> body {background-color: rgb(252, 243, 233);}"
		"h1 {color: rgb(200, 0, 0);}"
		"e1 {color: rgb(100, 0, 0);} </style>"
		"<body> <h1>ERROR</h1><br><e1>";
	body += size2Dec(_statusCode) + " " + statusCodes[_statusCode];
	body += "</e1></body></html>\n";

	return body;
};

std::string			&AMethod::generateIdxPage()
{
	DIR				*dir;
	struct dirent	*cur;
	std::string		body;

	dir = opendir(data.uri.script_name.c_str());
	if (!dir){
		_statusCode = errorOpeningURL;
		return ;
	}
	body = "<html><head><style> \
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
	while ((cur = readdir(dir)) != NULL){
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

	return body;
};
