// Common Gateway Interface. Headers in script:
// Content-Type - !MUST BE!
// Content-length
// Expires
// Location	- Redirection, probably we do not handle this case
// Pragma - * Turns document caching on and off.
// Status (cannot be sent as part of a complete header).
// class constructor input: **env in constructor, args, execpath;
// class methods: void cgiIn(const std::string &str); MethodStatus cgiOut(?);
// TODO: args: arg[0] = executable file; arg[1] = location root; arg[2] = NULL
#ifndef CGI_HPP
#define CGI_HPP
#include "include_resources.hpp"
#include <sys/types.h>
#include <sys/wait.h>

class CGI
{
	private:
		int pipein[2], pipeout[2]; // 2 pipes
		int pid; // fork id
		int status;
		stringMap _headersMap;
		void parseHeaders(std::string str);
	public:
		CGI(char *execpath, char **args, char **env); // prepare cgi process, prepare forks, etc
		void cgiInput(const std::string &str); // ready to input;
		std::string &cgiOut(); // ready to output
		~CGI();
};


#endif //CGI_HPP
