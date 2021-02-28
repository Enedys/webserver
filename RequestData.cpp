#include "RequestData.hpp"

RequestData::RequestData(t_ext_serv const &s, stringMap const &rHs, std::string const &uriR) :
	servsList(s), reqHeads(rHs), uri(uriR), errorMask(0), in(0) {serv = NULL; location = NULL; };

void		stringToLower(std::string &s)
{
	for (int i = 0; i < s.length(); i++)
		s[i] = std::tolower(s[i]);
}

void		RequestData::setHeaderState(headerNum hN, bool error)
{
	in |= hN;
	errorMask |= ((error != true) ? hN : 0);
}

void		RequestData::procQualityHeaders()
{
	constMapIter	header;
	qualityMap		m;

	if ((header = reqHeads.find("accept-charset")) != reqHeads.end())
	{
		m = parseAcceptionLine(header->second, 0, 1);
		setHeaderState(accChSet, m.first);
		acceptCharset = m.second;
	}
	if ((header = reqHeads.find("accept-language")) != reqHeads.end())
	{
		m = parseAcceptionLine(header->second, 1, 1);
		setHeaderState(accLang, m.first);
		acceptLanguage = m.second;
	}
	if ((header = reqHeads.find("content-language")) != reqHeads.end())
	{
		m = parseAcceptionLine(header->second, 1, 0);
		setHeaderState(contLang, m.first);
		contentLanguage = m.second;
	}
}

void		RequestData::procUserAgent()
{
	constMapIter	header;
	bool			var;
	if ((header = reqHeads.find("user-agent")) != reqHeads.end())
	{
		var = isValidUserAgent(header->second);
		setHeaderState(userAgent, var);
	}
}

void		RequestData::procContentType()
{
	constMapIter	header;
	bool			var;
	if ((header = reqHeads.find("content-type")) != reqHeads.end())
	{
		contTypeMap map = getContentType(header->second);
		contentType = map.second;
		setHeaderState(contType, map.first);
	}
}

void		RequestData::procHost()
{
	bool			var;
	hostName = reqHeads.find("host")->second;
	var = isValidHost(hostName, servsList.port);
	setHeaderState(host, var);
}

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

static int	getBase64Pos(int ch)
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
				chunk = getBase64Pos(s[4 * i + j]);
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

int			isValidQuerySymbol(int c)
{
	if (std::isalnum(c) || c == '-' ||\
		c == '_' || c == '.' || c == '~')
		return (okQueryCh);
	else if (c == '%')
		return (percent);
	else if (c == '=')
		return (equal);
	return (0);
}

int			isHex(int c)
{
	if (c >= 47 && c <= 57)			// digit
		return (c - '0');
	else if (c >= 97 && c <= 122)	// lowercase ascii
		return (c - 'a');
	else if (c >= 65 && c <= 90)	// uppercase ascii
		return (c - 'A');
	return (-1);
}

std::pair<std::string, int>	RequestData::getEnvVar(std::string const &s, size_t start)
{
	std::pair<std::string, int> res;
	res.second = -1;
	size_t	eqPos = s.find('=', start);
	if (eqPos == std::string::npos && start == 0)
		{setHeaderState(query, false); return (res);}
	size_t	delimPos = queryUri.find_first_of(";&", eqPos);
	if (delimPos == std::string::npos)
		delimPos = s.length();
	std::string	envVar = "HTTP_";
	envVar.reserve(5 + delimPos - start);
	int	validSym;
	int	i = 0;
	int	eqNum = 0;
	while (i < delimPos - start)
	{
		int	c = isValidQuerySymbol(s[i]);
		if (c == okQueryCh)
			envVar.push_back(s[i]);
		else if (c == equal)
			if (++eqNum == 1)
				envVar.push_back(s[i]);
		else
			{setHeaderState(query, false); return (res);}
		i++;
	}
	res.first = envVar;
	res.second = delimPos;
	return (res);
}

bool		RequestData::isValidPath()
{
	setHeaderState(path, true);
	return (true);
}

std::string	normalizeURI(std::string const &s)
{
	std::string	normUri;
	normUri.reserve(s.length());
	size_t	i = 0;
	while (i < s.length())
	{
		if (s[i] != '/')
			break ;
		else
			i++;
	}
	normUri.push_back('/');
	while (i < s.length())
	{
		if (s[i] == '%' && s.length() - i > 2)
		{
			int	c1 = isHex(s[i + 1]);
			int	c2 = isHex(s[i + 2]);
			if (c1 != -1 && c2 != -1)
			{
				normUri.push_back(c1 * 16 + c2);
				i += 2;
			}
			else
				normUri.push_back(s[i]);
		}
		else
			normUri.push_back(s[i]);
		i++;
	}
	return (normUri);
}

void		RequestData::uriParse(std::string const &uri, bool envNeed)
{
	std::string normUri = normalizeURI(uri);
	size_t	queryPos = normUri.find('?');
	pathFromUri = normUri.substr(0, queryPos);
	bool	val = isValidPath();
	if (queryPos != std::string::npos && val)
	{
		setHeaderState(query, true);
		size_t	fragmentPos = normUri.find('#', queryPos);
		queryUri = normUri.substr(queryPos + 1, fragmentPos - queryPos - 1);
		fragmentUri = normUri.substr(fragmentPos);
		if (!envNeed)
			return ;
		std::pair<std::string, int>	env = getEnvVar(queryUri, 0);
		while (env.second != std::string::npos && env.second != -1)
		{
			queryEnv.push_back(env.first);
			env = getEnvVar(queryUri, env.second + 1);
		}
	}
}

static int	match(std::string const &s1, std::string const &s2, size_t i1 = 0, size_t i2 = 0)
{
	if (i1 != s1.length() && s2[i2] == '*' && (i2 == 0 || i2 == s2.length() - 1))
		return (match(s1, s2, i1 + 1, i2) + match(s1, s2, i1, i2 + 1));
	else if (i1 == s1.length() && s2[i2] == '*' && (i2 == s2.length() - 1))
		return (1);
	else if (s1[i1] == s2[i2] && i1 != s1.length() && i2 != s2.length())
		return (match(s1, s2, i1 + 1, i2 + 1));
	else if (s1[i1] == s2[i2] && i1 == s1.length() && i2 == s2.length())
		return (1);
	return (0);
}

void		RequestData::determineServer()
{
	if ((errorMask & host) || (errorMask & path) || (errorMask & query)) // возможно нужна проверка только на существование хоста?
		{setHeaderState(servT, false); serv = NULL; return ;}
	size_t			portPos = hostName.find_last_of(':');
	std::string		_hostName = hostName.substr(0, portPos);
	stringToLower(_hostName);
	std::vector<t_serv>::const_iterator sv = servsList.servs.cend();
	for (std::vector<t_serv>::const_iterator i = servsList.servs.cbegin(); i < servsList.servs.cend(); i++)
	{
		std::string	tmpServName = i->serverName;
		stringToLower(tmpServName);
		if (match(_hostName, tmpServName))
			if (sv->serverName.length() < i->serverName.length()\
				|| sv == servsList.servs.cend())
				sv = i;
	}
	if (sv == servsList.servs.cend())
		serv = &(servsList.servs[0]);
	else
		serv = &(*sv);
	setHeaderState(servT, true);
}

bool		RequestData::findLocation()
{
	if (!serv)
		{setHeaderState(locFind, false); return (false);}
	constLocIter	itLoc = serv->locs.begin();
	constLocIter	itBest = serv->locs.end();
	size_t	pos = 0;
	while (itLoc != serv->locs.end())
	{
		if ((pos = uri.find(itLoc->path)) == std::string::npos)
			;
		if (itBest == serv->locs.end())
			itBest = itLoc;
		else if (itLoc->path.length() >= itBest->path.length())
			itBest = itLoc;
		itLoc++;
	}
	if (itBest == serv->locs.end())	/* what path should return if location for such uri does not exist ? */
	{
		// location = &(serv->locs[0]);
		location = NULL;
		setHeaderState(locFind, false);
		return (false);
	}
	else
		location = &(*itBest);
	size_t	root_len = itBest->root.length();
	if (itBest->root.back() == '/')
		pathToFile = itBest->root.substr(0, root_len - 1) + pathFromUri;
	else
		pathToFile = itBest->root + pathFromUri;
	setHeaderState(locFind, true);
	return (true);
}

std::string	getClientIp(in_addr_t ip)
{
	uint16_t x = 0x0001;
	std::string	clientIp;
	int n1, n2, n3, n4;
	n1 = ip >> 24;
	n2 = ((int32_t)(ip << 8)) >> 24;
	n3 = ((int32_t)(ip << 16)) >> 24;
	n4 = ((int32_t)(ip << 24)) >> 24;
	if (*((uint8_t *) &x))
	{
		int	tmp;
		tmp = n4;
		n4 = n1;
		n1 = tmp;
		tmp = n3;
		n3 = n2;
		n2 = tmp;
	}
	clientIp = size2Hex(n1, 10) + size2Hex(n2, 10) +size2Hex(n3, 10) + size2Hex(n4, 10);
	return (clientIp);
}

void		RequestData::getCGIconfig(size_t contLen, std::string method, sockaddr_in addr)
{
	if (!serv || !location || (errorMask & query))
		{setHeaderState(cgiE, false); return ;}
	size_t dotPos = pathToFile.find_last_of('.');
	std::string	extension = "";
	if (dotPos != std::string::npos)
		extension = pathToFile.substr(dotPos + 1); 
	if (location->cgi.find(extension) == location->cgi.end())
		{setHeaderState(cgiE, false); return ;}
	if (!(cgi_conf = (char **)malloc(18 * sizeof(char *))))
		{setHeaderState(cgiE, false); return ;}
	int	i = 0;
	if ((in & auth) && !(errorMask & auth))
		cgi_conf[i++] = strdup("AUTH_TYPE=Basic");
	cgi_conf[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	cgi_conf[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	cgi_conf[i++] = strdup("SERVER_SOFTWARE=BSHABILLUM/1.1");
	cgi_conf[i++] = strdup(("SERVER_NAME=" + serv->serverName).c_str());
	cgi_conf[i++] = strdup(("SERVER_PORT=" + size2Hex(serv->port, 10)).c_str());

	if ((in & query) && !(errorMask & query))
		cgi_conf[i++] = strdup(("QUERY_STRING=" + queryUri).c_str());
	cgi_conf[i++] = strdup(("REQUEST_URI=" + uri).c_str());
	if (!pathInfo.empty())
	{
		cgi_conf[i++] = strdup("PATH_INFO=");
		cgi_conf[i++] = strdup("PATH_TRANSLATED=");
	}
	cgi_conf[i++] = strdup(("SCRIPT_NAME=" + pathFromUri).c_str());

	cgi_conf[i++] = strdup(("CONTENT_TYPE=" + contentType["type"]).c_str());
	cgi_conf[i++] = strdup(("CONTENT_LENGTH=" + size2Hex(contLen, 10)).c_str());

	
	cgi_conf[i++] = strdup(("REMOTE_ADDR=" + getClientIp(addr.sin_addr.s_addr)).c_str());
	cgi_conf[i++] = strdup(("REQUEST_METHOD=" + method).c_str());
	if ((in & auth) && !(errorMask & auth) && !(errorMask & host))
	{
		std::string	username = location->authLogPass.substr(0,\
								location->authLogPass.find_first_of(':'));
		cgi_conf[i++] = strdup(("REMOTE_IDENT=" + username + '.' + hostName).c_str());
		cgi_conf[i++] = strdup(("REMOTE_USER=" + username).c_str());
	}
	cgi_conf[i] = NULL;
}

void		RequestData::procAuthorization()
{
	constMapIter	header;

	if ((header = reqHeads.find("authorization")) != reqHeads.end())
	{
		if (!location)
			{setHeaderState(auth, false); return ;}
		if (location->authLogPass == "" || location->auth == "")
			{setHeaderState(auth, true); return ;} // Пришел запрос на авторизацию, но она не требуется.
		std::string	authCont = header->second;
		size_t spacePos = authCont.find(' ', 0);
		if (spacePos == std::string::npos)
			{setHeaderState(auth, false); return ;}
		std::string	authType = authCont.substr(0, spacePos);
		stringToLower(authType);
		if (authType != "basic")
			{setHeaderState(auth, false); return ;}
		std::string encodedReq = authCont.substr(spacePos + 1);
		std::string encoded = base64encode(location->authLogPass);
		if (encoded == encodedReq)
			{setHeaderState(auth, true); return ;}
		else
			{setHeaderState(auth, false); return ;}
	}
}

void		RequestData::prepareData(size_t contLen, std::string method, sockaddr_in addr)
{
	procHost();
	procQualityHeaders();
	procUserAgent();
	procContentType();
	uriParse(uri, true);
	determineServer();
	findLocation();
	procAuthorization();
	getCGIconfig(contLen, method, addr);
}

