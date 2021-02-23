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
	std::map<std::string, std::pair<int, Server *> >	host_port;
	for (int i = 0; i < parser.servers.size(); i++)
	{
		std::cout << "Host: " << parser.servers[i].host << "\tServerName: " << parser.servers[i].serverName\
		<< "\tPort: " << parser.servers[i].port << std::endl;
		try
		{
			if (host_port[parser.servers[i].host].first != parser.servers[i].port)
			{
				myWebServer.appendServer(new Server(parser.servers[i]));
				host_port[parser.servers[i].host]. = parser.servers[i].port;
			}
			else
			{
				
			}
		}
		catch (char const *s)
		{
			std::cerr << s << std::endl;
			return (1);
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