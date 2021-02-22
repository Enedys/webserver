// Common Gateway Interface. Headers in script:
// Content-Type - !MUST BE!
// Content-length
// Expires
// Location	- Redirection, probably we do not handle this case
// Pragma - * Turns document caching on and off.
// Status (cannot be sent as part of a complete header).
// class input: **env in constructor.
// class methods: void cgiIn(const std::string &str); MethodStatus cgiOut(?);
#ifndef CGI_HPP
#define CGI_HPP


class CGI
{
	private:
		int pipein[2], pipeout[2]; // 2 pipes
	public:
		CGI(char **env); // prepare cgi process, prepare forks, etc
		~CGI();
};


#endif //CGI_HPP
