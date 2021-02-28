#include "CGI.hpp"

void CGI::initPipes()
{
	if (pipe(pipein) < 0)
	{
		throw CGI::pipeFailed();
	}
	if (pipe(pipeout) < 0)
	{
		close(pipein[0]);
		close(pipein[1]);
		throw CGI::pipeFailed();
	}
	status = 0;
	/*
	 * next we need to set fd as unblockable, so we won't hang at write function. Write will return -1 if pipe is full;
 	 */
	if (fcntl(pipein[0], F_SETFL, O_NONBLOCK) < 0)
	{
		freeMem();
		throw CGI::lockCanNotSet();
	}
	if (fcntl(pipeout[1], F_SETFL, O_NONBLOCK) < 0)
	{
		freeMem();
		throw CGI::lockCanNotSet();
	}
}

void CGI::initFork()
{
	if ((pid = fork()) < 0)
	{
		freeMem();
		throw CGI::forkFailed();
	}
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
}

CGI::CGI(char *execpath, char **args, char **env)
{
	initPipes();
	initFork();
	headersDone = false;
}

void CGI::input(const std::string &str) // inputting body
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

//MethodStatus CGI::output(std::string &str) // mb gonna change it later. Read and write into pipes and outputting. Less memory usage
//{
//	char buf[BUFSIZ];
//	size_t find;
//	int r;
//	r = read(pipeout[1], buf, BUFSIZ); // read < 0 = pipe is empty..
//	if (r < -1)
//	{
//		int wp = waitpid(pid, &status, WNOHANG); // returns > 0 if process stopped;
//		if (status == 1024)
//		{
//			freeMem();
//			return (error); // execve failed;
//		}
//		if (wp > 0)
//		{
//			freeMem();
//			return (ok);
//		}
//		else
//			return (inprogress);
//	}
//	if (!headersDone) // headers ready for parser
//	{
//		str = outputBuf + str;
//		if ((find = str.find("\n\n")) != std::string::npos)
//		{
//			parseHeaders(str.substr(0, find)); // todo: test how it works
//			str = str.substr(find + 2, str.size()); //
//			headersDone = true;
//		}
//		else
//		{
//			outputBuf = str;
//		}
//	}
//	else
//	{
//		str = buf;
//	}
//	inputFromBuf();
//	return (inprogress);
//}

MethodStatus CGI::output(std::string &str) // mb gonna change it later. Read and write into pipes and outputting. Less memory usage
{
	char buf[BUFSIZ];
	size_t find;
	int r;
	r = read(pipeout[1], buf, BUFSIZ); // read < 0 = pipe is empty..
	if (r < -1)
	{
		int wp = waitpid(pid, &status, WNOHANG); // returns > 0 if process stopped;
		if (status == 1024)
		{
			freeMem();
			return (error); // execve failed;
		}
		if (wp > 0)
		{
			freeMem();
			return (ok);
		}
		else
			return (inprogress);
	}
	if (!headersDone) // headers ready for parser
	{
		str = outputBuf + str;
		if ((find = str.find("\n\n")) != std::string::npos)
		{
			parseHeaders(str.substr(0, find)); // todo: test how it works
			str = str.substr(find + 2, str.size());
			outputBuf.clear();
			headersDone = true;
		}
		outputBuf = str;
	}
	else
	{
		str = outputBuf + buf;
		outputBuf.clear();
	}
	inputFromBuf();
	return (inprogress);
}

CGI::CGI()
{

}

CGI::~CGI()
{
	freeMem();
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

void CGI::freeMem()
{
	close(pipein[0]);
	close(pipeout[1]);
	close(pipein[1]);
	close(pipeout[0]);
	inputBuf.clear();
	outputBuf.clear();
}

void CGI::setExecpath(const char *expath) {
	CGI::execpath = expath;
}

void CGI::setArgs(char **argv) {
	CGI::args = argv;
}

void CGI::setEnv(char **argve) {
	CGI::env = argve;
}

void CGI::init()
{
	initPipes();
	initFork();
	headersDone = false;
}

bool CGI::isHeadersDone() const
{
	return headersDone;
}

const char *CGI::forkFailed::what() const throw()
{
	return ("Fork failed!");
}

const char *CGI::pipeFailed::what() const throw()
{
	return ("Pipe failed!");
}

const char *CGI::lockCanNotSet::what() const throw()
{
	return ("Lock can not set");
}
