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
// ** make the route able to accept uploaded files and configure where it should be saved
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
#include <algorithm>

typedef struct s_serv
{
	std::string host;
	int			port;
	bool		cgiToFile;
	std::map<int, std::string> error_pages;
	std::string serverName;
	int 		bodySizeLimit;
	std::string root;
	std::map<std::string, std::string> cgi;
	std::vector<struct s_loc> locs;
} t_serv;

struct s_loc
{
	std::string path; // rename to locPath?
	std::string root;////////error page
	std::string fileRequestIsDir;
	std::map<std::string, std::string> cgi;
	std::vector<std::string> index;
	std::string auth;
	std::string authLogPass;
	std::string uploadStore;
	int bodySizeLimit;
	bool cgiToFile;
	bool uploadPass;
	bool autoindex;
	bool getAvailable;
	bool postAvailable;
	bool headAvailable;
	bool putAvailable;
	bool optionsAvailable;
	bool deleteAvailable;
	bool cgiToFileSet;
};

typedef	struct s_ext_serv
{
	int						port;
	std::string				host;
	std::vector<t_serv>		servs;
}	t_ext_serv;

class Parser
{
	public:
		Parser();
		~Parser();
		Parser(const std::string &file);
		void parse(const std::string& file);
		std::vector<t_serv> servers;
		std::vector<t_ext_serv> servers_ext;
	private:
		class	s_comparator
		{
			public:
				bool	operator()(t_serv &a, t_serv &b) const
				{
					if (a.host != b.host)
						return (a.host < b.host);
					return (a.port < b.port);
				}
		};
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
		void 	getCgi();
		void 	getServerName();
		void 	getErrorPage();
		void 	getPageSize();
		void 	getCgiFile();
		void 	getLocRoot();
		void 	getLocAutoindex();
		void 	getLocFileIsDir();
		void 	getLocDenyMethod();
		void 	getLocCGI();
		void 	getLocAuth();
		void	getLocLogPass();
		void	getLocUploadPass();
		void 	getLocUploadStore();
		void	getLocIndex();
		void	getLocPageSize();
		void 	getLocCgiFile();
		void 	initServ();
		void 	initLoc();

		void	error(const std::string& msg);

		void 	validateErrorStr(const std::vector<std::string> &v);
		void 	fillRootLoc();
		void 	splitHost(const std::string &val);
		std::string getValue(const std::string &section);
		std::vector<std::string> getVectorValues(const std::string &section);

		void makeServExt();
};
#endif
