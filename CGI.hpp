// Common Gateway Interface. Headers in script:
// Content-Type - !MUST BE!
// Content-length
// Expires
// Location	- Redirection, probably we do not handle this case
// Pragma - * Turns document caching on and off.
// Status (cannot be sent as part of a complete header).
// class constructor input: **env in constructor, args, execpath;
// class methods: void cgiIn(const std::string &str); MethodStatus output(?);
// TODO: args: arg[0] = executable file; arg[1] = location root; arg[2] = NULL
#ifndef CGI_HPP
#define CGI_HPP
#include "include_resources.hpp"
#include "Header.hpp"
#include <sys/types.h>
#include <sys/wait.h>

enum cgiStatus
{
	not_started,
	running,
	done,
	failed
};

class CGI
{
	private:
		int pipein[2], pipeout[2]; // 2 pipes
		int pid; // fork id
		int processStatus;
		int tmpFile;
		bool headersDone;
		bool headersNotFound;
		bool headersNotFoundProcessExited;
		bool contentLength;
		bool headersSent;
		bool writeToFile;
		bool processDone; // when we are using file;
		std::string inputBuf;
		std::string sendBuf;
		std::string outputBuf; // Smaller string, just to get data to send;

		const char *execpath; // remove const?
		char **args;
		char **env;
		cgiStatus status;
		void parseHeaders(std::string str);
		void inputFromBuf();
		void freeMem();
		void initPipes();
		void initFork();
		void initForkFile();
		void initPipesFile();
		MethodStatus readFromProcess(std::string &str);
		MethodStatus sendOutput(std::string &output, int socket);
		MethodStatus outputChunkedFromBuf(std::string &str);
		MethodStatus outputContentLengthFromBuf(std::string &str);
		MethodStatus cgiProcessStatus();
		bool fileExists(char *filename);
		static const int maxChunkSize = BUFSIZ * 4;
		static const int maxContentLengthOutput = BUFSIZ * 4;
		MethodStatus cgiDone;
		std::string script_name;
		std::string root;
		Header *_header;
		char *ptrString;
		int ptrStrlen;
		int inpBytes;
		int outpBytes;
		int writePipe;
		bool cgiInternalError;
		static const int outpBufSize = BUFSIZ * 4;
	public:
		void setScriptName(const std::string &scriptName);
		void setRoot(const std::string &root);
		CGI();
		void init(); // if default constructor, this func need to be called;
		void input(const std::string &str, MethodStatus mStatus); // ready to input;
		MethodStatus output(std::string &str); // ready to output
		int init(RequestData const &data);
		MethodStatus getHeaders();
		MethodStatus outputChunked(std::string &);
		MethodStatus outputContentLength(std::string &);
		MethodStatus getHttpStatus();
		MethodStatus smartOutput(std::string &str);
		MethodStatus superSmartOutput(int socket);
		void concatHeaders();
		void setExecpath(const char *execpat);
		void setArgs(char **args);
		bool isHeadersDone() const;
		void setEnv(char **env);
		bool isHeadersNotFound() const;
		cgiStatus getStatus() const;

		~CGI();
		class pipeFailed: public std::exception
		{
				virtual const char *what() const throw();
		};
		class forkFailed: public std::exception
		{
				virtual const char *what() const throw();
		};
		class lockCanNotSet: public std::exception
		{
				virtual const char *what() const throw();
		};
		stringMap _headersMap;
		int httpStatus;
};


#endif //CGI_HPP
