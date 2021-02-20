#include "Method.hpp"

AMethod::~AMethod() {};

AMethod::AMethod(t_serv const &config, int &status, stringMap const &headers) : 
	_config(config), _statusCode(status), _headersMapRequest(headers) {};

const std::string	AMethod::validMethods[AMethod::methodNums] = {
"PUT", "POST", "DELETE", "OPTION", "GET", "HEAD"
};

bool				AMethod::isValidMethod(std::string const &method)
{
	for (int i = 0; i < AMethod::methodNums; i++)
		if (AMethod::validMethods[i] == method)
			return (true);
	return (false);
}

MethodStatus	AMethod::sendHeader(int socket)
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

