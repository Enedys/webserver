#include "CGI.hpp"

CGI::CGI(char *execpath, char **args, char **env)
{
	// initialasing the pipe
	pipe(pipein); // TODO: error handle
	pipe(pipeout); // TODO: error handle
	pid = fork(); // TODO: if pid < 0 error;
	if (pid == 0)
	{
		dup2(pipein[1], 0);
		close(pipein[1]);
		dup2(pipeout[0], 1);
		close(pipeout[0]);
		execve(execpath, args, env);
	}
//	waitpid(pid, &status, WUNTRACED);
}

void CGI::cgiInput(const std::string &str)
{
	int r;
	r = write(pipein[0], str.c_str(), str.length());
}

std::string &CGI::cgiOut()
{
	char buf[BUFSIZ];
	std::string str;
	size_t find;
	int r;
	r = read(pipeout[1], buf, BUFSIZ); // TODO: read > 0
	str = buf;
	if ((find = str.find("\n\n")) != std::string::npos) // headers ready for parser
	{
		parseHeaders(str.substr(0, find)); // todo: test how it works
		str = str.substr(find + 2, str.size()); //
		if (!str.empty())
			std::cout << "body is ready!";
		std::cout << "body is missing!";
	}
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