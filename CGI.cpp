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
	processStatus = 0;
	/*
	 * next we need to set fd as unblockable, so we won't hang at write function. Write will return -1 if pipe is full;
 	 */
	if (fcntl(pipein[1], F_SETFL, O_NONBLOCK) < 0)
	{
		freeMem();
		throw CGI::lockCanNotSet();
	}
	if (fcntl(pipeout[0], F_SETFL, O_NONBLOCK) < 0)
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
		dup2(pipein[0], 0);
		close(pipein[0]);
		dup2(pipeout[1], 1);
		close(pipeout[1]);
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
	headersNotFound = false;
	headersNotFoundProcessExited = false;
	httpStatus = -1;
	contentLength = false;
	headersSent = false;
}

void CGI::input(const std::string &str, MethodStatus mStatus) // inputting body
{
	int r;
	if (inputBuf.empty() && mStatus == ok)///status check // status == ok
	{
		close(pipein[1]);//return
	}
	if (inputBuf.empty())
	{
		r = write(pipein[1], str.c_str(), str.length());
		if (r == -1) // pipe is absolutely full
			inputBuf += str;
		else if (r < str.size()) // pipe is full;
			inputBuf += str.substr(r, str.size());
	}
	else
	{
		r = write(pipein[1], inputBuf.c_str(), 8192); // TODO: set bufsize
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
		r = write(pipein[1], inputBuf.c_str(), 8192); // TODO: set bufsize
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
	bzero(buf, BUFSIZ);
	size_t find;
	int r;
	r = read(pipeout[0], buf, BUFSIZ); // read < 0 = pipe is empty..
	if (r < 0)
	{
		int wp = waitpid(pid, &processStatus, WNOHANG); // returns > 0 if process stopped;
		if (processStatus == 1024)
		{
			freeMem();
			return (error); // execve failed;
		}
		if (wp > 0 || headersNotFoundProcessExited) // 2 times WNOHANG = wp = -1; TODO: test || headersNotFound
		{
//			freeMem();
			if (!headersDone) // means, headers not found, just output as it is
			{
				headersDone = true;
				headersNotFoundProcessExited = true;
			}
			else
			{
				str = outputBuf;
				freeMem();
			}
			return (ok);
		}
		else
			return (inprogress);
	}
	std::cout << "!!!!\n\n";
	// std::cout << buf << std::endl;
	if (!headersDone) // headers ready for parser
	{
		str = outputBuf + buf;
		if ((find = str.find("\r\n\r\n")) != std::string::npos)
		{
			parseHeaders(str.substr(0, find + 2)); // todo: test how it works
			str = str.substr(find + 4, str.size());
			outputBuf.clear();
			headersDone = true;
		}
		else if (str.size() > 16000) // todo: CLARIFY
		{
			headersDone = true;
			headersNotFound = true;
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
	while (str.find("\r\n") != std::string::npos)
	{
		key = str.substr(0, str.find(':'));
		for (int i = 0; i < key.length(); i++)
			key.at(i) = std::tolower(key.at(i));
		value = str.substr(str.find(':') + 1, str.find('\n') - (str.find(':') + 1)); // todo: test
		value = value.substr(value.find_first_not_of(" \v\t"), value.size()); // test ' '
		str = str.substr(str.find("\r\n") + 2, str.size());
		if (key == "status")
		{
			value = value.substr(0, 3);
			httpStatus = std::atoi(value.c_str());
			continue;
		}
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
	this->execpath = expath;
}

void CGI::setArgs(char **argv) {
	this->args = argv;
}

void CGI::setEnv(char **argve) {
	CGI::env = argve;
}

void CGI::init()
{
	initPipes();
	initFork();
	headersDone = false;
	headersNotFound = false;
	headersNotFoundProcessExited = false;
	contentLength = false;
	headersSent = false;
	httpStatus = -1;
}

bool CGI::isHeadersDone() const
{
	return headersDone;
}

bool CGI::isHeadersNotFound() const
{
	return (this->headersNotFound || this->headersNotFoundProcessExited);
}

cgiStatus CGI::getStatus() const // depricated
{
	return status;
}

MethodStatus CGI::getHeaders()
{
	char buf[BUFSIZ];
	bzero(buf, BUFSIZ);
	std::string str;
	size_t find;
	int r;
	r = read(pipeout[0], buf, BUFSIZ); // read < 0 = pipe is empty..
	if (r < 0)
	{
		int wp = waitpid(pid, &processStatus, WNOHANG); // returns > 0 if process stopped;
		if (processStatus == 1024)
		{
			freeMem();
			return (error); // execve failed;
		}
		if (wp > 0 || headersNotFoundProcessExited) // 2 times WNOHANG = wp = -1; TODO: test || headersNotFound
		{
//			freeMem();
			if (!headersDone) // means, headers not found, just output as it is
			{
				headersDone = true;
				headersNotFoundProcessExited = true;
			}
			else
			{
				str = outputBuf; // TODO: just save it for later
				freeMem();
			}
			return (ok);
		}
		else
			return (inprogress);
	}
	if (!headersDone) // headers ready for parser
	{
		str = outputBuf + buf;
		if ((find = str.find("\r\n\r\n")) != std::string::npos)
		{
			parseHeaders(str.substr(0, find + 2)); // todo: test how it works
			str = str.substr(find + 4, str.size());
			outputBuf.clear();
			headersDone = true;
			outputBuf = str;
			inputFromBuf(); // todo: need?
			if (httpStatus < 0)
				httpStatus = 200; // if parseHeaders did not update http status, set it to 200;
			concatHeaders();
			return ok;
		}
		else if (str.size() > 16000) // todo: CLARIFY
		{
			headersDone = true;
			headersNotFound = true;
		}
		outputBuf = str;
	}
	inputFromBuf();
	return inprogress;
}

MethodStatus CGI::outputChunked(std::string &str)
{
	std::string buf;
	if (outputBuf.length() > maxChunkSize)
	{
		buf = outputBuf.substr(0, maxChunkSize); // todo: clarify for special characters
		outputBuf = outputBuf.substr(maxChunkSize, outputBuf.length());
		str = size2Hex(buf.length()) + "\r\n" + buf + "\r\n";
		return inprogress;
	}
	else
	{
		str = size2Hex(outputBuf.length()) + "\r\n" + outputBuf + "\r\n";
		return ok; // TODO: process can be still working, add inputfrombuf;
	}
}

MethodStatus CGI::outputContentLength(std::string &str)
{
	if (outputBuf.length() > maxContentLengthOutput)
	{
		str = outputBuf.substr(0, maxContentLengthOutput);
		outputBuf = outputBuf.substr(maxContentLengthOutput, outputBuf.length());
		return inprogress;

	}
	str = outputBuf;
	return ok;
}

void CGI::concatHeaders()
{
	stringMap headersMapCommon;
	_header = new Header(script_name, root, httpStatus);
	_header->createGeneralHeaders(headersMapCommon);
	std::map<std::string, std::string>::iterator itCommon;
	std::map<std::string, std::string>::iterator it;
	for (itCommon = headersMapCommon.begin(); itCommon != headersMapCommon.end(); itCommon++)
	{
		it = _headersMap.find(itCommon->first);
		if (it != _headersMap.end())
		{
			_headersMap.insert(std::pair<std::string, std::string>(itCommon->first, itCommon->second));
		}
	}
	if (_headersMap.find("content-length") != _headersMap.end())
		contentLength = true;

}

MethodStatus CGI::smartOutput(std::string &str)
{
	if (!headersDone)
	{
		return (getHeaders());
		str.clear();
	}
}

MethodStatus CGI::sendOutput(std::string &output, int socket)
{
	int r;
	r = send(socket, output.c_str(), output.length(), MSG_DONTWAIT);
	if (r < output.length())
	{
		sendBuf = output.substr(r, output.length());
		return inprogress;
	}
	sendBuf.clear();
	return ok;
}

MethodStatus CGI::superSmartOutput(int socket)
{
	std::string str;
	int r;
	MethodStatus mStatus;
	if (!headersDone)
	{
		return getHeaders();
	}
	if (!sendBuf.empty())
	{
		sendOutput(sendBuf, socket);
		return inprogress;
	}
	if (!headersSent)
	{
		_header->headersToString(_headersMap, str);
		headersSent = true;
		sendOutput(str, socket);
		return inprogress;
	}
	if (contentLength)
		mStatus = outputContentLength(str);
	else
		mStatus = outputChunked(str);
	if (sendOutput(str, socket) == ok)
		return mStatus;
	else
		return inprogress;
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
