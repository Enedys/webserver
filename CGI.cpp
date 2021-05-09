#include "CGI.hpp"

void CGI::initPipes()
{
	std::cout << " -- INIT PIPES -- \n";
	if (pipe(pipein) < 0)
	{
		status = failed;
		throw CGI::pipeFailed();
	}
	if (!writeToFile)
	{
		if (pipe(pipeout) < 0)
		{
			status = failed;
			close(pipein[0]);
			close(pipein[1]);
			throw CGI::pipeFailed();
		}
		if (fcntl(pipeout[0], F_SETFL, O_NONBLOCK) < 0)
		{
			freeMem();
			status = failed;
			throw CGI::lockCanNotSet();
		}
	}
	else
	{
		tmpFile = open(".", O_TMPFILE | O_RDWR, 0644);
		pipeout[0] = tmpFile;
	}
	if (fcntl(pipein[1], F_SETFL, O_NONBLOCK) < 0)
	{
		freeMem();
		status = failed;
		throw CGI::lockCanNotSet();
	}
	processStatus = 0;
}

void CGI::initFork()
{
	status = running;
	if ((pid = fork()) < 0)
	{
		freeMem();
		status = failed;
		throw CGI::forkFailed();
	}
	if (pid == 0)
	{
		for (int i = 3; i < 1024; i++)
		{
			if (i != pipein[0] && i != pipein[1] && i != pipeout[0] && i != pipeout[1] && i != tmpFile)
				close(i);
		}
		dup2(pipein[0], 0);
		close(pipein[0]);
		close(pipein[1]);
		if (!writeToFile)
		{
			dup2(pipeout[1], 1);
			close(pipeout[1]);
			close(pipeout[0]);
		}
		else
		{
			dup2(tmpFile, 1);
			close(tmpFile);
		}
		execve(execpath, args, env);
		std::cout << "ALARM! EXECVE FAILED!\n"; // what to do?
		close(0);
		close(1);
		close(2);
		exit(4);
	}
	else
	{
		std::cout << "PID: " << pid << std::endl;
		std::cout << "PIPEIN[0]: " << pipein[0] << std::endl;
		std::cout << "PIPEIN[1]: " << pipein[1] << std::endl;
		std::cout << "PIPEOUT[0]: " << pipeout[0] << std::endl;
		std::cout << "PIPEOUT[1]: " << pipeout[1] << std::endl;
//		close(pipein[1]);
//		pipein[1] = -1;
//		close(pipeout[0]);
//		pipeout[0] = -1;
	}

}

void CGI::init()
{
	pipein[0] = -1;
	pipein[1] = -1;
	pipeout[0] = -1;
	pipeout[1] = -1;
	inpBytes = 0;
	outpBytes = 0;
	writePipe = 0;
	tmpFile = -1;
	processDone = false;
	status = not_started;
	initPipes();
	initFork();
	headersDone = false;
	cgiInternalError = false;
	headersNotFound = false;
	headersNotFoundProcessExited = false;
	contentLength = false;
	headersSent = false;
	cgiDone = inprogress;
	//inputBuf.clear();
	outputBuf.clear();
	sendBuf.clear();
	httpStatus = -1;
}

void CGI::input(const std::string &str, MethodStatus mStatus) // inputting body
{
	int r;

	if (str.empty() && inputBuf.empty() && mStatus == ok)
	{
		std::cout << "CLOSE PIPEIN[1]: " << pipein[1] << std::endl;
		close(pipein[1]);//return
		pipein[1] = -1;
		return ;
	}
	inpBytes += str.length();
	if (inputBuf.empty())
	{
		r = write(pipein[1], str.c_str(), str.size());
		if (r > 0)
		{
			std::cout << "Send to pipe" << writePipe << std::endl;
			writePipe += r;
			std::cout << "Send to pipe" << writePipe << std::endl;
		}
		if (r == -1) // pipe is absolutely full
			inputBuf += str;
		else if (r < static_cast <int> (str.size())) // pipe is full;
			inputBuf += str.substr(r, str.size());
		else if (r == static_cast <int> (str.size()) && mStatus == ok)
		{
			std::cout << "CLOSE PIPEIN[1]: " << pipein[1] << std::endl;
			close(pipein[1]);
			pipein[1] = -1;
		}
	}
	else
	{
		r = write(pipein[1], inputBuf.c_str(), inputBuf.size()); // TODO: set bufsize
		if (r > 0)
		{
			std::cout << "Send to pipe" << writePipe << std::endl;
			writePipe += r;
			std::cout << "Send to pipe" << writePipe << std::endl;
		}
		if (r > 0 && r <  static_cast <int> (inputBuf.size()))
			inputBuf.erase(0, r);
		else if (r == static_cast <int> (inputBuf.size()))
		{
			inputBuf.clear();
			if (str.empty() && mStatus == ok)
			{
				std::cout << "CLOSE PIPEIN[1]: " << pipein[1] << std::endl;
				close(pipein[1]);
				pipein[1] = -1;
			}
		}
		inputBuf += str;
	}
	if (mStatus == ok)
	{
		std::cout << "WROTE TO PIPE: " << writePipe << std::endl;
		std::cout << inputBuf.size() << std::endl;
		ptrString = (char *)inputBuf.c_str();
		ptrStrlen = (int)inputBuf.size();
		//sleep(10);
	}
}

//void CGI::inputFromBuf()
//{
//	if (!inputBuf.empty())
//	{
//		int r;
//		r = write(pipein[1], inputBuf.c_str(), inputBuf.size()); // TODO: set bufsize
//		if (r > 0)
//		{
//			std::cout << "Send to pipe" << writePipe << std::endl;
//			writePipe += r;
//			std::cout << "Send to pipe" << writePipe << std::endl;
//		}
//		if (r > 0 && r < static_cast <int> (inputBuf.size()))
//			inputBuf.erase(0, r);
//		else if (r == static_cast <int> (inputBuf.size()))
//		{
//			inputBuf.clear();
//			std::cout << "CLOSE PIPEIN[1]: " << pipein[1] << std::endl;
//			close(pipein[1]); // todo: SIMPLIFY IFS?
//			pipein[1] = -1;
//		}
//	}
//}

void CGI::inputFromBuf()
{
	if (!inputBuf.empty())
	{
		int r;
		r = write(pipein[1], ptrString, ptrStrlen); // TODO: set bufsize
		if (r > 0)
		{
			std::cout << "Send to pipe" << writePipe << std::endl;
			writePipe += r;
			std::cout << "Send to pipe" << writePipe << std::endl;
			ptrStrlen -= r;
		}
		if (r > 0 && r < ptrStrlen)
			ptrString += r;
		else if (r == ptrStrlen)
		{
			inputBuf.clear();
			std::cout << "CLOSE PIPEIN[1]: " << pipein[1] << std::endl;
			close(pipein[1]); // todo: SIMPLIFY IFS?
			pipein[1] = -1;
		}
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


MethodStatus CGI::cgiProcessStatus()
{
	std::string str;
	int wp = waitpid(pid, &processStatus, WNOHANG); // returns > 0 if process stopped;
	if (processStatus == 1024 || (processStatus > 128 && processStatus < 160)) // todo: clarify
	{
		//freeMem();
		cgiInternalError = true;
		status = failed;
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
			str = outputBuf; // todo: !!!!!!!
			freeMem();
		}
		status = done;
		return (ok);
	}
	else
		return (inprogress);
}

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
	pipein[0] = -1;
	pipein[1] = -1;
	pipeout[0] = -1;
	pipeout[1] = -1;
	args = NULL;
	status = not_started;
	headersDone = false;
	headersNotFound = false;
	headersNotFoundProcessExited = false;
	contentLength = false;
	headersSent = false;
	cgiDone = inprogress;
	outputBuf.clear();
	sendBuf.clear();
	httpStatus = -1;
}

CGI::~CGI()
{
	std::cout << " -- CGI DESTR -- \n";
	freeMem();
	//cgiProcessStatus();
	//sleep(1);
	//sleep(10);
}

void CGI::parseHeaders(std::string str)
{
	std::string key;
	std::string value;
	while (str.find("\r\n") != std::string::npos)
	{
		key = str.substr(0, str.find(':'));
		for (int i = 0; i <  static_cast <int> (key.length()); i++)
			key.at(i) = std::tolower(key.at(i));
		value = str.substr(str.find(':') + 1, str.find("\r\n") - (str.find(':') + 1)); // todo: test; upd: looks, like works
		value = value.substr(value.find_first_not_of(" \v\t"), value.size()); // test ' '
		str = str.substr(str.find("\r\n") + 2, str.size());
		if (key == "status")
		{
			value = value.substr(0, 3);
			httpStatus = std::atoi(value.c_str());
			continue;
		}
		if (_headersMap.find(key) != _headersMap.end())
		{
			_headersMap[key] += std::string("\r\n" + key + ": " + value); // todo: test later, seems work;
		}
		else
			_headersMap.insert(std::pair<std::string, std::string>(key, value));
	}
}

void CGI::freeMem()
{
	if (pipein[0] != -1)
	{
		close(pipein[0]);
		std::cout << "CLOSE PIPEIN[0]: " << pipein[0] << std::endl;
	}
	if (pipein[1] != -1)
	{
		std::cout << "CLOSE PIPEIN[1]: " << pipein[1] << std::endl;
		close(pipein[1]);
	}
	if (pipeout[0] != -1)
	{
		std::cout << "CLOSE PIPEOUT[0]: " << pipeout[0] << std::endl;
		close(pipeout[0]);
	}
	if (pipeout[1] != -1)
	{
		std::cout << "CLOSE PIPEOUT[1]: " << pipeout[1] << std::endl;
		close(pipeout[1]);
	}



	pipein[0] = -1;
	pipein[1] = -1;
	pipeout[1] = -1;
	pipeout[0] = -1;
	inputBuf.clear();
	outputBuf.clear();
	if (args)
		free(args);
	args = NULL;
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


bool CGI::isHeadersDone() const
{
	return headersDone;
}

bool CGI::isHeadersNotFound() const
{
	return (this->headersNotFound || this->headersNotFoundProcessExited);
}

//cgiStatus CGI::getStatus() const // depricated
//{
//	return status;
//}

MethodStatus CGI::getHeaders()
{
	char buf[outpBufSize + 1];
	bzero(buf, outpBufSize + 1);
	std::string str;
	size_t find;
	int r;
	inputFromBuf(); // todo: temporary
	r = read(pipeout[0], buf, outpBufSize); // read < 0 = pipe is empty..
	if (writeToFile && r == 0)
	{
		if (headersNotFoundProcessExited)
		{
			httpStatus = 200;
			concatHeaders();
		}
		return ok;
	}
	if (r < 0)
	{
		MethodStatus st = cgiProcessStatus();
		if (headersNotFoundProcessExited)
		{
			httpStatus = 200;
			concatHeaders();
		}
		return st;
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
			{
				httpStatus = 200; // if parseHeaders did not update http status, set it to 200;
			}
			concatHeaders();
			outpBytes = outputBuf.size();
			return ok;
		}
		else if (str.size() > 16000) // todo: CLARIFY
		{
			headersDone = true;
			headersNotFound = true;
			httpStatus = 200;
			outputBuf = str;
			concatHeaders();
			return ok;
		}
		outputBuf = str;
	}
	inputFromBuf();
	return inprogress;
}

MethodStatus CGI::outputChunkedFromBuf(std::string &str)
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
		outputBuf.clear();
		return ok; // TODO: process can be still working, add inputfrombuf;
	}
}

MethodStatus CGI::outputContentLengthFromBuf(std::string &str)
{
	if (outputBuf.length() > maxContentLengthOutput)
	{
		str = outputBuf.substr(0, maxContentLengthOutput);
		outputBuf = outputBuf.substr(maxContentLengthOutput, outputBuf.length());
		return inprogress;
	}
	str = outputBuf;
	outputBuf.clear();
	return ok;
}
MethodStatus CGI::readFromProcess(std::string &str)
{
	char buf[outpBufSize + 1];
	bzero(buf, outpBufSize + 1);
	int r;
	r = read(pipeout[0], buf, outpBufSize);
	if (r > 0)
		outpBytes += r;
	std::cout << "Bytes output: " << outpBytes << " out of " << inpBytes << std:: endl;
	if (writeToFile && r == 0)
	{
		str.clear();
		return ok;
	}
	if (r < 0)
	{
		str.clear();
		return cgiProcessStatus();
	}
	if (!contentLength)
	{
		std::string kekw;
		kekw = buf;
		str = size2Hex(kekw.length()) + "\r\n" + kekw + "\r\n";
	}
	else
		str = buf;
	inputFromBuf(); // TODO: insert it SOMEWHERE;
	return inprogress;
}


MethodStatus CGI::outputChunked(std::string &str)
{
	if (!outputBuf.empty())
	{
		outputChunkedFromBuf(str);
		return inprogress;
	}
	return readFromProcess(str);
}

MethodStatus CGI::outputContentLength(std::string &str)
{
	if (!outputBuf.empty())
	{
		outputContentLengthFromBuf(str);
		return inprogress;
	}
	return readFromProcess(str);
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
		std::string key = itCommon->first;
		for (int i = 0; i <  static_cast <int> (key.length()); i++)
			key.at(i) = std::tolower(key.at(i)); // todo: temporary
		it = _headersMap.find(key);
		if (it == _headersMap.end())
			_headersMap.insert(std::pair<std::string, std::string>(itCommon->first, itCommon->second));
	}
	if (_headersMap.find("content-length") != _headersMap.end())
		contentLength = true;
	else if (_headersMap.find("transfer-encoding") == _headersMap.end())
		_headersMap.insert(std::pair<std::string, std::string>("transfer-encoding", "chunked"));
	//delete (_header);
}

MethodStatus CGI::sendOutput(std::string &output, int socket)
{
	int r;
	r = send(socket, output.c_str(), output.length(), MSG_DONTWAIT);
	if (r <  static_cast <int> (output.length()))
	{
		sendBuf = output.substr(r, output.length());
		return inprogress;
	}
	sendBuf.clear();
	return ok;
}

MethodStatus CGI::smartOutput(std::string &str)
{
	str.clear();
	MethodStatus mStatus;
	if (cgiInternalError)
	{
		if (!headersSent)
		{
			httpStatus = 500;
			concatHeaders();
			_header->headersToString(_headersMap, str);
			headersSent = true;
			delete _header;
			return inprogress;
		}
		str = "0\r\n\r\n";
		return ok;
	}
	if (writeToFile && !processDone)
	{
		inputFromBuf();
		mStatus = cgiProcessStatus();
		if (mStatus == error)
			return inprogress;
		else if (mStatus == ok)
		{
			headersDone = false;
			headersNotFoundProcessExited = false;
			lseek(tmpFile, 0L, SEEK_SET);
			processDone = true;
		}
		return inprogress;
	}
	if (!headersDone)
	{
		getHeaders();
		str.clear();
		return inprogress;
	}
	if (!headersSent)
	{
		_header->headersToString(_headersMap, str);
		delete _header;
		headersSent = true;
		return inprogress;
	}
	if (contentLength)
		mStatus = outputContentLength(str);
	else
		mStatus = outputChunked(str);
	if (str.empty() && (mStatus == ok || writeToFile) && !contentLength)
	{
		str = "0\r\n\r\n";
		mStatus = ok;
	}
	return mStatus;
}


// todo: test headers not found and process exited;

MethodStatus CGI::superSmartOutput(int socket)
{
	std::string str;
	MethodStatus mStatus;
	if (!headersDone)
	{
		getHeaders();
		return inprogress;
	}
	if (!sendBuf.empty())
	{
		if (sendOutput(sendBuf, socket) == ok && cgiDone == ok && !contentLength)
			return (ok);
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
	if (str.empty() && mStatus == ok && !contentLength)
	{
		str = "0\r\n\r\n";
		cgiDone = ok;
		return sendOutput(str, socket);
	}
//	if (sendOutput(str, socket) == ok)
//		return mStatus;
//	else
	sendOutput(str, socket);
	return inprogress;
}

void CGI::setScriptName(const std::string &scriptName)
{
	script_name = scriptName;
}

void CGI::setRoot(const std::string &root)
{
	CGI::root = root;
}

cgiStatus CGI::getStatus() const
{
	return status;
}

MethodStatus CGI::getHttpStatus()
{
	if (!headersDone)
	{
		getHeaders();
		return inprogress;
	}
	return ok;
}

int CGI::init(const RequestData &data)
{
	args = (char **)malloc(sizeof(char *) * 3);	
	args[0] = (char *)data.cgi_bin.c_str();
	args[1] = (char *)data.uri.script_name.c_str();
	args[2] = NULL;
	if (data.location->cgiToFile)
		writeToFile = true;
	else
		writeToFile = false;
	setExecpath(args[0]);
	setRoot(data.location->root); // todo: need?
	setScriptName(data.uri.script_name); // todo: need?
	setEnv(data.cgi_conf);
	init();
	return 200;
}

bool CGI::fileExists(char *filename)
{
	struct stat   buffer;
	return (stat(filename, &buffer) == 0);
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
