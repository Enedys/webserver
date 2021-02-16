// #include "Server.hpp"
#include "WebServer.hpp"

int main()
{
	try{

		//Server myServer("127.0.0.1", 3535);
		Parser	parser;
		parser.parse("webserv.conf");
		WebServer	myWebServer;
		std::cout << parser.servers.size() << std::endl;
		for (int i = 0; i < parser.servers.size(); i++)
		{
			std::cout << "Host: " << parser.servers[i].host << "\nServerName: " << parser.servers[i].serverName\
			<< "\nPort: " << parser.servers[i].port << std::endl;
			myWebServer.appendServer(new Server(parser.servers[i]));
		}
		myWebServer.showServerList();
		// std::cout << myServer << std::endl;
		// myWebServer.appendServer(myServer);
		// myServer.setRoot("/");
		// myServer.appendLocation("/webserver/files", "/Users/kwillum");
		// myServer.appendLocation("/webserver/files/ft_webserver", "");
		// std::string	fileUri = myServer.getUri("/webserver/files/ft_webserver/test.cpp");
		// std::cout << fileUri << std::endl;
		myWebServer.runWebServer();
	}
	catch(char const *s)
	{
		std::cout << s << std::endl;
	}
	return (0);
}