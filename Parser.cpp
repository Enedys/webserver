//
// Created by abibi on 02.02.2021.
//

#include "Parser.hpp"
#include <cstdlib>
Parser::Parser()
{

}

Parser::~Parser()
{

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
			std::cout << "123!\n"; // TODO: ??
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
	//in   = &i;

/*
	int tmpin;
	int fd;

	tmpin = dup(0);
	fd = open(fin.c_str(), O_RDONLY);
	dup2(fd, 0);
	close(fd);
	in = &std::cin;
	Token t;
	std::string value;
	getNextToken(value);
	while (nextToken != FILE_END)
	{
		if (nextToken == NEWLINE)
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
	close(tmpin); */

	struct s_loc loc;
	t_serv val1, val2;
	val1.host = "127.0.0.1";
	val1.port = 5000;
	val1.bodySizeLimit = 1048576;
	val1.root = getcwd(0, 0);
	val1.error_pages.insert(std::pair<int, std::string>(404, "/custom_404.html"));
	val1.error_pages.insert(std::pair<int, std::string>(404, "/custom_400.html"));
	loc.path = "/";
	loc.root = val1.root;
	loc.autoindex = false;
	loc.getAvailable = true;
	loc.postAvailable = true;
	loc.headAvailable = true;
	loc.putAvailable = false;
	val1.locs.push_back(loc);
	
	loc.path = "/test";
	loc.root = getcwd(0, 0);
	loc.root += "/tata";
	loc.autoindex = false;
	loc.getAvailable = true;
	loc.postAvailable = true;
	loc.headAvailable = true;
	loc.putAvailable = true;
	val1.locs.push_back(loc);
	servers.push_back(val1);


	val2.host = "127.0.0.2";
	val2.port = 9911;
	val2.bodySizeLimit = 1048576;
	val2.root = getcwd(0, 0);
	val2.error_pages.insert(std::pair<int, std::string>(404, "/custom_404.html"));
	val2.error_pages.insert(std::pair<int, std::string>(404, "/custom_400.html"));
	loc.path = "/";
	loc.root = val2.root + "/tata";
	loc.autoindex = false;
	loc.getAvailable = false;
	loc.postAvailable = true;
	loc.getAvailable = true;
	loc.headAvailable = true;
	loc.putAvailable = true;
	val2.locs.push_back(loc);
	servers.push_back(val2);
}


void Parser::getRoot()
{
	std::string value;
	Token t = getNextToken(value);
	while (t == WHITESPACE)
		t = getNextToken(value);
	if (t != IDENTIFIER)
		error("Er_2");
	root = value;
	t = getNextToken(value);
	if (t != SEMICOLON)
		error("Er_3");
	std::cout << "root done!" << root << "\n";

}

void Parser::getHost()
{
	std::string value;
	Token t = getNextToken(value);
	while (t == WHITESPACE)
		t = getNextToken(value);
	if (t != IDENTIFIER)
		error("Er_2");
	host = value;
	t = getNextToken(value);
	if (t != SEMICOLON)
		error("Er_3");
	std::cout << "host done!\n";
}

void Parser::getServerName()
{
	std::string value;
	Token t = getNextToken(value);
	while (t == WHITESPACE)
		t = getNextToken(value);
	if (t != IDENTIFIER)
		error("Er_2");
	serverName = value;
	t = getNextToken(value);
	if (t != SEMICOLON)
		error("Er_3");

	std::cout << "servername done!\n";
}

void Parser::getErrorPage()
{

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
	}
}

void Parser::parseServer()
{
	std::string value;
	Token t = getNextToken(value);
	if (t != IDENTIFIER || value != "server")
		error("Er_00!");
	t = getNextToken(value);
	if (t != OPEN_BRACE)
	{
		error("Expected {");
	}
	while (nextToken == IDENTIFIER || nextToken == NEWLINE || nextToken == WHITESPACE)
	{
		parseValues();
	}
	if (nextToken != CLOSE_BRACE)
	{
		error("er_05");
	}
	else
		getNextToken(value);
}

void Parser::parseLocation()
{

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


