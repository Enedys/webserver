#include "Client.hpp"
#include "Server.hpp"
#include "WebServer.hpp"

int main()
{
	try{
		Server myServer("127.0.0.1", 7512);
		std::cout << myServer << std::endl;
		WebServer	myWebServer;
		myWebServer.appendServer(myServer);
		myWebServer.showServerList();
		myWebServer.runWebServer();
	}
	catch(char const *s)
	{
		std::cout << s << std::endl;
	}
	return (0);
}