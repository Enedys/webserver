// * choose the port and host of each "server"
// * setup the server_names or not
// * The first server for a host:port will be the default for this host:port (meaning it will answer to all request that doesn’t belong to an other server)
// * setup default error pages
// * limit client body size
// * setup routes with one or multiple of the following rules/configuration (routes wont be using regexp):
// ** define a list of accepted HTTP Methods for the route
// ** define a directory or a file from where the file should be search (for example if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is //tmp/www/pouic/toto/pouet)
// ** autoindex
// ** default file to answer if the request is a directory
// ** execute CGI based on certain file extension (for example .php)
// tokens: server_name, listen, location, root, autoindex, error_page [NUM], {, }
// listen -> HOST:IP
// server_name -> STRING
#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <map>

typedef struct s_serv
{
	std::string host;
	int			port;
	std::map<int, std::string> error_pages;
	std::string serverName;
	int 		bodySizeLimit;
	std::vector<struct s_loc> locs;
} t_serv;

struct s_loc
{
	std::string path; // rename to locPath?
	std::string root;
	std::string fileRequestIsDir;
	bool autoindex;
	bool getAvailable;
	bool postAvailable;
	bool headAvailable;
	bool putAvailable;
};

class Parser
{
	public:
		Parser();
		~Parser();
		void parse(const std::string& file);
		std::vector<t_serv> servers;

	private:
		enum Token
		{
			NO_TOKEN,
			WHITESPACE,
			SEMICOLON,
			OPEN_BRACE,  // {
			CLOSE_BRACE, // {
			IDENTIFIER,
			NEWLINE,
			FILE_END,
			COMMENT,
			ERROR
		};
		Token nextToken;
		std::string nextValue;

		std::istream* in;

		t_serv serv;
		struct s_loc loc;
		std::string root;

		Token getNextToken(std::string &value);
		Token lexToken(std::string &value);
		Token lexWhitespace(std::string &value);
		Token lexNewline(std::string &value);
		Token lexIdentifier(std::string &value);
		Token lexComment(std::string &value);

		void 	parseServer();
		void	parseValues();
		void 	parseLocValues();
		void 	parseLocation();
		void 	getRoot();
		void 	getHost();
		void 	getServerName();
		void 	getErrorPage();
		void 	getPageSize();
		void 	getLocRoot();
		void 	getLocAutoindex();
		void 	getLocFileIsDir();
		void 	getLocDenyMethod();
		void 	initServ();
		void 	initLoc();

		void	error(const std::string& msg);

		int 	validateErrorStr(const std::string &str);
		void 	fillRootLoc();
		void 	splitHost(const std::string &val);
};


#endif
