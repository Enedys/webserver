// #include "Server.hpp"
#include "WebServer.hpp"

void				printLog(Logger *_webLogger, std::string msg, int addInfo, Message::LogLevel lvl)
{
	if (_webLogger && addInfo != -1)
			*_webLogger << lvl << msg << addInfo << Logger::endl;
	else if (_webLogger && addInfo == -1)
		*_webLogger << lvl << msg << Logger::endl;
}

int main()
{

	//Server myServer("127.0.0.1", 3535);
	Parser	parser;
	parser.parse("webserv.conf");
	
	WebServer	myWebServer;
	std::cout << parser.servers.size() << std::endl;
	// return (0);

	for (int i = 0; i < parser.servers.size(); i++)
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