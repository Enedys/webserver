#include "Parser.hpp"
#include <cstdlib>
Parser::Parser()
{

}

Parser::~Parser()
{

}


Parser::Parser(const std::string &file)
{
	parse(file);
}

Parser::Token Parser::getNextToken(std::string &value)
{
	Token token = nextToken;
	value = nextValue;

	nextToken = lexToken(nextValue);
	while (nextToken == NEWLINE || nextToken == COMMENT)
		nextToken = lexToken(nextValue);
	return token;
}

Parser::Token Parser::lexToken(std::string &value)
{
	value.clear();
	int c = in->get();
	switch (c)
	{
		case ' ': case '\t': case '\v':
			value.push_back(c);
			return lexWhitespace(value);
		case '\n': case '\r':
			value.push_back(c);
			return lexNewline(value);
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
		case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
		case 's': case 't': case 'u': case 'v': case 'w': case 'x':
		case 'y': case 'z':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
		case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
		case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
		case 'Y': case 'Z':
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '_': case '-': case '/': case ':': case '.':
			value.push_back(c);
			return lexIdentifier(value);
		case '{':
			value = "{";
			return OPEN_BRACE;
		case '}':
			value = "}";
			return CLOSE_BRACE;
		case ';':
			value = ";";
			return SEMICOLON;
		case '#':
			return lexComment(value);
		case -1:
			return FILE_END;
		default:
			value.push_back(c);
			std::cout << "!!!" << c << std::endl;
			error("Unexpected " + value + ".");
			return ERROR;
	}
}

Parser::Token Parser::lexWhitespace(std::string &value)
{
	int c = in->get();
	while (true)
	{
		switch (c)
		{
			case ' ': case '\t': case '\v':
				value.push_back(c);
				break;
			default:
				in->unget();
				return WHITESPACE;
		}
		c = in->get();
	}
}

Parser::Token Parser::lexNewline(std::string &value)
{
	int c = in->get();
	switch (c)
	{
		case '\n': case '\r':
			if (c != value[0])
			{
				value.push_back(c); // \r\n or \n\r
			}
			else
			{
				in->unget(); // treat \n \n as two newline
			}
			return NEWLINE;
		default:
			in->unget();
			return NEWLINE;
	}
}


Parser::Token Parser::lexIdentifier(std::string &value)
{
	int c = in->get();
	while (true)
	{
		switch (c)
		{
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
			case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
			case 's': case 't': case 'u': case 'v': case 'w': case 'x':
			case 'y': case 'z':
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
			case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
			case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
			case 'Y': case 'Z':
			case '_': case '-': case '/': case ':': case '.':
				value.push_back(c);
				break;
			default:
				in->unget();
				return IDENTIFIER;
		}
		c = in->get();
	}
}

void Parser::parse(const std::string& fin)
{
	int tmpin;
	int fd;

	tmpin = dup(0);
	fd = open(fin.c_str(), O_RDONLY);
	if (fd < 0)
		error("Can't open file " + fin);
	dup2(fd, 0);
	close(fd);
	in = &std::cin;
	std::string value;
	getNextToken(value);
	while (nextToken != FILE_END)
	{
		if (nextToken == NEWLINE || nextToken == WHITESPACE) // WHITESPACES?
		{
			// skip empty lines
			getNextToken(value);
		}
		else
		{
			parseServer();
		}
	}
	dup2(tmpin, 0);
	close(tmpin);
}

std::string Parser::getValue(const std::string &section)
{
	std::string value;
	std::string ret;
	Token t = getNextToken(value);
	while (t == WHITESPACE || t == NEWLINE)
		t = getNextToken(value);
	if (t != IDENTIFIER)
		error(section + "expected value");
	ret = value;
	t = getNextToken(value);
	while (t == WHITESPACE || t == NEWLINE)
		t = getNextToken(value);
	if (t != SEMICOLON)
		error(section + "expected ;");
	return ret;
}

std::vector<std::string> Parser::getVectorValues(const std::string &section)
{
	std::string value;
	Token t = getNextToken(value);
	std::vector<std::string> vecValues;
	while (t != SEMICOLON)
	{
		t = getNextToken(value);
		if (t == IDENTIFIER)
			vecValues.push_back(value);
		else if (t == WHITESPACE || t == SEMICOLON)
			continue ;
		else
			error(section + "invalid token");
	}
	return vecValues;
}


void Parser::getRoot()
{
	root = getValue("server: root: ");
}

void Parser::getHost()
{
	std::string value = getValue("server: listen: ");
	splitHost(value);
}

void Parser::getServerName()
{
	serv.serverName = getValue("server: server_name: ");
}

void Parser::getErrorPage()
{
	std::vector<std::string> vecValues = getVectorValues("server: error_page ");
	validateErrorStr(vecValues); // error: program will exit in func
	for (unsigned int i = 0; i < vecValues.size() - 1; i++)
		serv.error_pages.insert(std::pair<int, std::string>(std::atoi(vecValues[i].c_str()), vecValues[vecValues.size() - 1]));
}

void Parser::getPageSize()
{
	std::string value = getValue("server: page_size: ");
	int num;

	num = std::atoi(value.c_str());
	for (unsigned int i = 0; i < value.size() - 1; i++)
		if (value[i] < '0' || value[i] > '9')
			error("server: page_size: bad number");
	if (value[value.size() - 1] == 'M')
		num *= 1048576;
	else if (value[value.size() - 1] == 'K')
		num *= 1024;
	else if (value[value.size() - 1] < '0' || value[value.size() - 1] > '9')
		error("server: page_size: bad number. Expected 'M' or 'K' or digit");
	serv.bodySizeLimit = num;
}

void Parser::parseValues()
{
	std::string value;
	Token t = getNextToken(value);

	if (t == NEWLINE)
		return ;
	if (t == WHITESPACE)
		return ;
	if (t != IDENTIFIER)
		error("No identifier!");
	if (value == "location")
		parseLocation();
	else
	{
		if (value == "root")
			getRoot();
		else if (value == "listen")
			getHost();
		else if (value == "error_page")
			getErrorPage();
		else if (value == "server_name")
			getServerName();
		else if (value == "page_size")
			getPageSize();
		else
			error ("Server: invalid token " + value);
	}
}

void Parser::parseServer()
{
	initServ();
	std::string value;
	Token t = getNextToken(value);
	if (t != IDENTIFIER || value != "server")
		error("expected server");
	t = getNextToken(value);
	while (t == WHITESPACE || t == NEWLINE)
		t = getNextToken(value);
	if (t != OPEN_BRACE)
	{
		error("Expected {");
	}
	while (nextToken == IDENTIFIER || nextToken == NEWLINE || nextToken == WHITESPACE)
	{
		parseValues();
	}
	if (nextToken == OPEN_BRACE)
	{
		error("second { in server section");
	}
	if (nextToken != CLOSE_BRACE)
	{
		error("expected close brace for server section");
	}
	else
	{
		fillRootLoc(); // Fill location roots if empty
		servers.push_back(serv);
		getNextToken(value);
	}
}

void Parser::getLocCGI()
{
	std::vector<std::string> vecValues = getVectorValues("location: cgi: "); // TODO: error management .php .py
	for (unsigned int i = 0; i < vecValues.size() - 1; i++)
		loc.cgi.insert(std::pair<std::string, std::string>(vecValues[i], vecValues[vecValues.size() - 1]));
}

void Parser::getLocRoot()
{
	loc.root = getValue("location: root: ");
}

void Parser::getLocAutoindex()
{
	std::string value = getValue("location: autoindex: ");
	if (value == "off")
		loc.autoindex = false;
	else if (value == "on")
		loc.autoindex = true;
	else
		error("autoindex is off/on; not anything else");
}

void Parser::getLocFileIsDir()
{
	loc.fileRequestIsDir = getValue("location: reqisdir: ");
}

void Parser::getLocDenyMethod()
{
	std::string value = getValue("location: deny: ");
	if (value == "GET")
		loc.getAvailable = false;
	else if (value == "POST")
		loc.postAvailable = false;
	else if (value == "HEAD")
		loc.headAvailable = false;
	else if (value == "PUT")
		loc.putAvailable = false;
	else
		error("expected deny POST | HEAD | PUT | GET");
}

void Parser::parseLocValues()
{
	std::string value;
	Token t = getNextToken(value);

	if (t == NEWLINE)
		return ;
	if (t == WHITESPACE)
		return ;
	if (t != IDENTIFIER)
		error("No identifier!");
	if (value == "root")
		getLocRoot();
	else if (value == "autoindex")
		getLocAutoindex();
	else if (value == "deny")
		getLocDenyMethod();
	else if (value == "reqisdir")
		getLocFileIsDir(); // error?
	else if (value == "cgi")
		getLocCGI();
	else
		error("Location: invalid token");
}

void Parser::parseLocation()
{
	std::string value;
	Token t = getNextToken(value);
	initLoc();

	if (t != WHITESPACE)
		error("Expected space after location");
	while (t == WHITESPACE)
		t = getNextToken(value);
	loc.path = value;
	if (nextToken != WHITESPACE)
		error("Expected space after path in location");
	t = getNextToken(value);
	while (t == WHITESPACE || t == NEWLINE) // todo: test
		t = getNextToken(value);
	if (t != OPEN_BRACE)
		error("Expected { after location");
	while (nextToken == IDENTIFIER || nextToken == NEWLINE || nextToken == WHITESPACE)
	{
		parseLocValues();
	}
	if (nextToken != CLOSE_BRACE)
	{
		error("er_08!!");
	}
	else
	{
		serv.locs.push_back(loc);
		getNextToken(value);
	}
}

void Parser::error(const std::string &msg)
{
	std::cout << msg << std::endl;
	exit(2);
}

Parser::Token Parser::lexComment(std::string &value)
{
	int c = in->get();
	while (true)
	{
		switch (c)
		{
			case '\n': case '\r':
				return COMMENT;
			default:
				value.push_back(c);
				break;
		}
		c = in->get();
	}
}

void Parser::initServ()
{
	serv.bodySizeLimit = 1048576;
	serv.error_pages.clear();
	serv.serverName.clear();
	serv.host.clear();
	serv.locs.clear();
}

void Parser::initLoc()
{
	loc.autoindex = false;
	loc.root.clear();
	loc.fileRequestIsDir.clear();
	loc.getAvailable = true;
	loc.headAvailable = true;
	loc.postAvailable = true;
	loc.putAvailable = true;
	loc.path.clear();
}

void Parser::validateErrorStr(const std::vector<std::string> &v)
{
	for (unsigned int i = 0; i < v.size() - 1; i++)
	{
		if (v[i].length() != 3)
			error("Error value in error_page not contains 3 symbols");
		for (unsigned int j = 0; j < v[i].length(); j++)
			if (v[i][j] < '0' || v[i][j] > '9')
				error("Error value in error_page contains bad characters");
	}
}

void Parser::fillRootLoc()
{
	for (unsigned int i = 0; i < serv.locs.size(); i++)
	{
		if (serv.locs[i].root.empty())
		{
			if (root.empty())
				error("location has no root. Can't resolve");
			serv.locs[i].root = root;
		}
	}
}

void Parser::splitHost(const std::string &val)
{
	size_t pos;
	std::string host;
	std::string port;
	pos = val.find(':');
	host = val.substr(0, pos); // TODO: error management
	port = val.substr(pos + 1, val.size());
	serv.host = host;
	serv.port = std::atoi(port.c_str());
}