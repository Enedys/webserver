#include "CGI.hpp"

CGI::CGI(char *execpath, char **args, char **env)
{
	// initialasing the pipe
	pipe(pipein);
	pipe(pipeout); // TODO: error handle
	status = 0;
	/*
	 * next we need to set fd as unblockable, so we won't hang at write function. Write will return -1 if pipe is full;
 	 */
	fcntl(pipein[0], F_SETFL, O_NONBLOCK);
	fcntl(pipeout[1], F_SETFL, O_NONBLOCK); // TODO: can return error;
	pid = fork(); // TODO: if pid < 0 error;
	if (pid == 0)
	{
		dup2(pipein[1], 0);
		close(pipein[1]);
		dup2(pipeout[0], 1);
		close(pipeout[0]);
		execve(execpath, args, env);
		std::cout << "ALARM! EXECVE FAILED!\n"; // what to do?
		exit(4);
	}
	headersDone = false;
}

void CGI::cgiInput(const std::string &str) // inputting body
{
	int r;
	if (inputBuf.empty())
	{
		r = write(pipein[0], str.c_str(), str.length());
		if (r == -1) // pipe is absolutely full
			inputBuf += str;
		else if (r < str.size()) // pipe is full;
			inputBuf += str.substr(r, str.size());
	}
	else
	{
		r = write(pipein[0], inputBuf.c_str(), 8192); // TODO: set bufsize
		if (r > 0 && r < inputBuf.size())
			inputBuf = inputBuf.substr(r, inputBuf.size());
		inputBuf += str;
	}
}

void CGI::inputFromBuf()
{
	if (!inputBuf.empty())
	{
		int r;
		r = write(pipein[0], inputBuf.c_str(), 8192); // TODO: set bufsize
		if (r > 0 && r < inputBuf.size())
			inputBuf = inputBuf.substr(r, inputBuf.size());
	}
}

MethodStatus CGI::cgiOut(std::string &str) // mb gonna change it later. Read and write into pipes and outputting. Less memory usage
{
	char buf[BUFSIZ];
	size_t find;
	int r;
	r = read(pipeout[1], buf, BUFSIZ); // read < 0 = pipe is empty..
	if (r < -1)
	{
		int wp = waitpid(pid, &status, WNOHANG); // returns > 0 if process stopped;
		if (status == 1024)
			return (error); // execve failed;
		if (wp > 0)
		{
			close(pipein[0]);
			close(pipeout[1]);
			// next code: ?
			close(pipein[1]);
			close(pipeout[0]);
			return (ok);
		}
		else
			return (inprogress);
	}
	str = buf;
	if (!headersDone && (find = str.find("\n\n")) != std::string::npos) // headers ready for parser
	{
		parseHeaders(str.substr(0, find)); // todo: test how it works
		str = str.substr(find + 2, str.size()); //
		headersDone = true;
	}
	inputFromBuf();
	return (inprogress);
}

CGI::~CGI()
{

}

void CGI::parseHeaders(std::string str)
{
	size_t pos;
	std::string key;
	std::string value;
	while (str.find('\n') != std::string::npos)
	{
		key = str.substr(0, str.find(':') - 1);
		for (int i = 0; i < key.length(); i++)
			key.at(i) = std::tolower(key.at(i));
		value = str.substr(str.find(':'), str.find('\n'));
		value = value.substr(value.find_first_not_of(" \v\t"), value.size()); // test ' '
		str = str.substr(str.find('\n'), str.size());
		_headersMap.insert(std::pair<std::string, std::string>(key, value));
	}
}