// #include "Server.hpp"
#include "WebServer.hpp"

void				printLog(Logger *_webLogger, std::string msg, int addInfo, Message::LogLevel lvl)
{
	if (_webLogger && addInfo != -1)
			*_webLogger << lvl << msg << addInfo << Logger::endl;
	else if (_webLogger && addInfo == -1)
		*_webLogger << lvl << msg << Logger::endl;
}

mapIntStr scInit()
{
	mapIntStr sc;
	sc[200] = "OK";
	sc[201] = "Created";
	sc[202] = "Accepted";
	sc[400] = "Bad Request";
	sc[401] = "Unauthorized";
	sc[403] = "Forbidden";//when autoindex is off and request is a folder
	sc[404] = "Not Found";
	sc[405] = "Method Not Allowed";
	sc[406] = "Not Acceptable";
	sc[408] = "Request Timeout";
	sc[411] = "Length Required";
	sc[414] = "URI Too Long";
	sc[415] = "Unsupported Media Type";
	sc[418] = "I'm a teapot";
	sc[421] = "Misdirected Request";
	sc[422] = "Unprocessable Entity";
	sc[429] = "Too Many Requests";
	sc[431] = "Request Header Fields Too Large";
	sc[449] = "Retry With";
	sc[499] = "Client Closed Request";
	sc[500] = "Internal Server Error";
	sc[501] = "Not Implemented";
	sc[503] = "Service Unavailable";//перегрузка сервера
	sc[505] = "HTTP Version Not Supported";
	return sc;
};

mapIntStr sc = scInit();

int main()
{
	//Server myServer("127.0.0.1", 3535);
	Parser	parser;
	parser.parse("webserv.conf");

	WebServer	myWebServer;
	std::cout << parser.servers.size() << std::endl;
	// return (0);

	for (int i = 0; i < parser.servers_ext.size(); i++)
	{
		std::cout << "Host: " << parser.servers_ext[i].host << "\tPort: " << parser.servers_ext[i].port << std::endl;
		try
		{
			myWebServer.appendServer(new Server(parser.servers_ext[i]));
		}
		catch (...)
		{
			std::cerr << "Creation server error" << std::endl;
			return (1);
		}
	}
	myWebServer.showServerList();
	myWebServer.runWebServer();
	return (0);
}