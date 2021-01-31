#include "Client.hpp"
#include "Server.hpp"
#include "WebServer.hpp"

int main()
{
	try{
		Server myServer("127.0.0.1", 3535);
		std::cout << myServer << std::endl;
		WebServer	myWebServer;
		myWebServer.appendServer(myServer);
		// myWebServer.showServerList();
		myServer.setRoot("/");
		myServer.appendLocation("/webserver/files", "/Users/kwillum");
		myServer.appendLocation("/webserver/files/ft_webserver", "");
		std::string	fileUri = myServer.getUri("/webserver/files/ft_webserver/test.cpp");
		std::cout << fileUri << std::endl;
		myWebServer.runWebServer();
	}
	catch(char const *s)
	{
		std::cout << s << std::endl;
	}
	return (0);
}