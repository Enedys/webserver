#include "RequestData.hpp"

RequestData::RequestData(t_ext_serv const &s, stringMap const &rHs) :
	servs(s), reqHeads(rHs), errorMask(0), in(0) {};

bool		RequestData::isValidHost(std::string const &s1, size_t port)
{
	size_t			portPos = s1.find_last_of(':');
	if (portPos != std::string::npos)
	{
		std::string portReq = s1.substr(portPos);
		std::string servPort = size2Hex(port, 10);
		if (portReq != servPort)
			return (false);
	}
	std::string		s = s1.substr(0, portPos);
	int	labelSize = 0;
	int	minusNum = 0;
	if (s.size() > 253 || s.size() == 0)
		return (false);
	int i = 0;
	while (i < s.size())
	{
		if (!std::isalnum(s[i]))
			return (false);
		while (std::isalnum(s[i]) || (s[i] == '-' && minusNum < 1))
		{
			if (s[i] == '-')
				minusNum++;
			i++;
			labelSize++;
			if (labelSize >= 64)
				return (false);
		}
		if (s[i] == '.' && i == s.size() - 1)
			return (false);
		if (s[i] == '.' && labelSize > 0 && s[i - 1] != '-')
			i++;
		labelSize = 0;
		minusNum = 0;
	}
	return (true);
}

bool		RequestData::isValidUserAgent(std::string const &s)
{
	size_t i = 0;
	int		bs = 0;
	while (i < s.length())
	{
		if (s[i] == '/')
		{
			if (i > 0)
			{
				if (s[i - 1] != '/' && s[i - 1] != ' ' && s[i - 1] != '\t' &&\
					s[i + 1] != '/' && s[i + 1] != ' ' && s[i + 1] != '\t')
					bs = 1;
			}
			else
				return (false);
		}
		else if ((s[i] == ' ' || s[i] == '\t') && !bs)
			return (false);
		if (isascii(s[i]))
			i++;
		else
			return (false);
	}
	return (bs > 0);
}

qualityMap	RequestData::parseAcceptionLine(std::string const &s, int isLang, int isQ)
{
	qualityMap	lst;
	size_t i = 0;
	if (s.length() == 0)
	{
		lst.first = false;
		return (lst);
	}
	lst.first = true;
	while (i < s.length())
	{
		bool	ok = true;
		int		q = -1;
		int		aster = 0;
		std::string	name = "";
		if (!std::isalpha(s[i]) && s[i] != '*')
			ok = false;
		size_t	start = i;
		while (s[i] != ',' && s[i] != '\0')
		{
			if (!ok)
				{i++; continue ;}
			if (std::isalnum(s[i]))
				i++;
			else if (s[i] == '*' && i == start)
				i++;
			else if (s[i] == '-' && std::isalnum(s[i - 1]) && std::isalnum(s[i + 1]))
				i++;
			else if ((s[i++] == ';') && isQ)
			{
				name = s.substr(start, i - 1 - start);
				if (isLang)
					name = name.substr(0, name.find('-'));
				if (s[i++] != 'q' && s[i++] != 'Q')
					{ok = false; continue ;}
				if (s[i++] != '=')
					{ok = false; continue ;}
				if (s[i] == '0' || s[i] == '1')
					q = s[i++] - '0';
				else
					{ok = false; continue ;}
				if (s[i] == ',' || s[i] == '\0')
					continue ;
				if (s[i] == '.' && s[i - 1] == '0')
					i++;
				else
					{ok = false; continue ;}
				int mul = 1000;
				for (int j = 0; j < 4; j++)
				{
					mul /= 10;
					if (std::isdigit(s[i]) && j < 3)
						q =  q + mul * (s[i] - '0');
					else if ((s[i] == ',' && j != 0))
						break ;
					else if (s[i] == '\0')
						break ;
					else
						{ok = false; break ;}
					i++;
				}

			}
			else
				ok = false;
		}
		if (ok && q == -1)
		{
			name = s.substr(start, i - start);
			if (isLang)
				name = name.substr(0, name.find('-'));
			q = 1000;
		}
		if (ok)
			lst.second[name] = std::max(lst.second[name], q);
		if (s[i++] == ',')
		{
			if (s[i] != ' ' && s[i] != '\t')
				ok = false;
			while (i < s.length() && (s[i] == ' ' || s[i] == '\t'))
				i++;

		}
		lst.first &= ok;
	}
	return (lst);
};

std::string	RequestData::base64encode(std::string const &s)
{
	char	alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char	num;
	unsigned char	res;
	std::string		encoded;
	encoded.reserve(s.size() / 3 * 4 + s.size() % 3 + 2);
	for (int i = 0; i < s.size(); i++)
	{
		unsigned char	ch = s[i];
		if (i % 3 == 0)
		{
			num =  ch >> 2;
			res = (unsigned char)(ch << 6) >> 2;
		}
		else if (i % 3 == 1)
		{
			num = (ch >> 4) | res;
			res = (unsigned char)(ch << 4) >> 2;
		}
		else if (i % 3 == 2)
		{
			num = (unsigned char)(ch >> 6) | res;
			res = (unsigned char)(ch << 2) >> 2;
		}
		encoded.push_back(alphabet[num]);
		if (i % 3 == 2)
			encoded.push_back(alphabet[res]);
	}
	if (s.size() % 3 != 0)
		encoded.push_back(alphabet[res]);
	if (s.size() % 3 != 0)
		encoded.push_back('=');
	if (s.size() % 3 == 1)
		encoded.push_back('=');
	return (encoded);
}

static int	getPos(int ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return (ch - 'A');
	else if (ch >= 'a' && ch <= 'z')
		return (26 + ch - 'a');
	else if (ch >= '0' && ch <= '9')
		return (52 + ch - '0');
	else if (ch == '+')
		return (62);
	else
		return (63);
}

std::string	RequestData::base64decode(std::string const &s)
{
	if (s.size() % 4 != 0)
		return ("");
	unsigned int	chunk;
	std::string		decode;
	decode.reserve((s.size() / 4 + 1) * 3);
	for (int i = 0; i < s.size() / 4; i++)
	{
		int res = 0;
		for (int j = 0; j < 4; j++)
		{
			if (s[4 * i + j] == '=')
				chunk = 0;
			else
				chunk = getPos(s[4 * i + j]);
			res += chunk * (1 << (6 * (3 - j)));
		}
		decode.push_back((res / 256) / 256);
		decode.push_back((res / 256) % 256);
		if (!(chunk == 0 && i == s.size() / 4 - 1))
			decode.push_back(res % 256);
	}
	return (decode);
}

contTypeMap	RequestData::getContentType(std::string const &s)
{
	contTypeMap	m;
	m.first = false;
	if (s.length() < 3)
		return (m);
	size_t	delim = s.find(';');
	std::string type = s.substr(0, delim);
	int delimNum = 0;
	for (int i = 0; i < type.length(); i++)
	{
		if (type[i] == '/' && delimNum++ < 1)
		{
			delimNum++;
			if (i == 0 || i == type.length() - 1)
				return (m);
		}
		else if (!std::isalnum(type[i]) && (type[i] != '-'))
			return (m);
	}
	m.second["type"] = type;
	if (delim == std::string::npos || (delim == s.length() - 2))
		{ m.first = true; return (m); }
	std::string ns = s.substr(delim + 1);
	size_t chs = ns.find("charset=");
	size_t next_del = 0;
	if (chs == std::string::npos)
	{
		chs = ns.find("boundary=");
		if (chs == std::string::npos && ns.length())
			return (m);
	}
	else
	{
		next_del = ns.find_first_of(";\0", chs);
		std::string encode = ns.substr(chs + 8, next_del - (chs + 8));
		m.second["charset"] = encode;
	}
	chs = ns.find("boundary=");
	if (chs == std::string::npos)
		{ m.first = true; return (m); }
	size_t next_del2 = ns.find_first_of(";\0", chs);
	if (next_del == next_del2)
		return (m);
	std::string boundary = ns.substr(chs + 9, next_del2 - (chs + 9));
	m.second["boundary"] = boundary;
		{ m.first = true; return (m); }
}

void		RequestData::prepareData()
{
	bool			var;
	constMapIter	header;
	var = isValidHost(reqHeads.find("host")->second, servs.port);
	errorMask |= (host & (var != true ? 1 : 0));
	in |= host;
	hostName = reqHeads.find("host")->second;
	if ((header = reqHeads.find("user-agent")) != reqHeads.end())
	{
		in |= userAgent;
		var = isValidUserAgent(header->second);
		errorMask |= (userAgent & (var != true ? 1 : 0));
	}
	qualityMap m; 
	if ((header = reqHeads.find("accept-charset")) != reqHeads.end())
	{
		in |= accChSet;
		m = parseAcceptionLine(header->second, 0, 1);
		errorMask |= (accChSet & (m.first != true ? 1 : 0));
		acceptCharset = m.second;
	}
	if ((header = reqHeads.find("accept-language")) != reqHeads.end())
	{
		in |= accLang;
		m = parseAcceptionLine(header->second, 1, 1);
		errorMask |= (accLang & (m.first != true ? 1 : 0));
		acceptLanguage = m.second;
	}
	if ((header = reqHeads.find("content-language")) != reqHeads.end())
	{
		in |= contLang;
		m = parseAcceptionLine(header->second, 1, 0);
		errorMask |= (contLang & (m.first != true ? 1 : 0));
		acceptLanguage = m.second;
	}
	if ((header = reqHeads.find("content-type")) != reqHeads.end())
	{
		in |= contType;
		contTypeMap map = getContentType(header->second);
		errorMask |= (contType & (map.first != true ? 1 : 0));
		contentType = map.second;
	}

}

