#include "RequestData.hpp"

RequestData::RequestData(t_ext_serv const *s,\
						stringMap const *rHs,\
						stringMap const *rFl) :
	_servsList(s), _reqHeads(rHs), errorMask(0), in(0)
{
	_uri = &rFl->find("uri")->second;
	_method = &rFl->find("method")->second;
	serv = NULL;
	location = NULL;
	cgi_conf = NULL;
	badalloc_index = -1;
};

RequestData::RequestData()
{
	location = NULL;	serv = NULL;
	_servsList = NULL;	_uri = NULL;
	_reqHeads = NULL;	_method = NULL;
	cgi_conf = NULL;
	error_code = 0;	errorMask = 0;	in = 0;
	badalloc_index = -1;
}

void		RequestData::cleanData()
{
	location = NULL;	serv = NULL;
	_servsList = NULL;	_uri = NULL;
	_reqHeads = NULL;	_method = NULL;
	hostName = "";
	acceptLanguage.clear();		acceptCharset.clear();
	contentLanguage.clear();	contentType.clear();
	errorMask = 0;	in = 0;	error_code = 0;
	uri.cleanData();
	cleanCGIenv();
	badalloc_index = -1;
	cgi_conf = NULL;
}

void		RequestData::setData(t_ext_serv const *s,\
									stringMap const *rHs,\
									stringMap const *rFl)
{
	_servsList = s;
	_reqHeads = rHs;
	_uri = &rFl->find("uri")->second;
	_method = &rFl->find("method")->second;
}

RequestData::~RequestData() {cleanCGIenv();};

void		RequestData::setHeaderState(headerNum hN, bool error)
{
	in |= hN;
	errorMask |= ((error != true) ? hN : 0);
}

void		RequestData::procQualityHeaders()
{
	constMapIter	header;
	qualityMap		m;

	if ((header = _reqHeads->find("accept-charset")) != _reqHeads->end())
	{
		m = parseAcceptionLine(header->second, 0, 1);
		setHeaderState(e_accChSet, m.first);
		acceptCharset = m.second;
	}
	if ((header = _reqHeads->find("accept-language")) != _reqHeads->end())
	{
		m = parseAcceptionLine(header->second, 1, 1);
		setHeaderState(e_accLang, m.first);
		acceptLanguage = m.second;
	}
	if ((header = _reqHeads->find("content-language")) != _reqHeads->end())
	{
		m = parseAcceptionLine(header->second, 1, 0);
		setHeaderState(e_contLang, m.first);
		contentLanguage = m.second;
	}
}

void		RequestData::procUserAgent()
{
	constMapIter	header;
	bool			var;
	if ((header = _reqHeads->find("user-agent")) != _reqHeads->end())
	{
		var = isValidUserAgent(header->second);
		setHeaderState(e_userAgent, var);
	}
}

void		RequestData::procContentType()
{
	constMapIter	header;
	bool			var;
	if ((header = _reqHeads->find("content-type")) != _reqHeads->end())
	{
		contTypeMap map = getContentType(header->second);
		contentType = map.second;
		setHeaderState(e_contType, map.first);
	}
}

void		RequestData::procHost()
{
	bool			var;
	hostName = _reqHeads->find("host")->second;
	var = isValidHost(hostName, _servsList->port);
	setHeaderState(e_host, var);
}

bool		RequestData::isValidHost(std::string const &s1, size_t port)
{
	size_t			portPos = s1.find_last_of(':');
	if (portPos != std::string::npos)
	{
		std::string portReq = s1.substr(portPos + 1);
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

static std::string
			getClientIp(in_addr_t ip)
{
	uint16_t	x = 0x0001;
	std::string	clientIp;
	int n1, n2, n3, n4;
	n1 = ip >> 24;
	n2 = ((int32_t)(ip << 8)) >> 24;
	n3 = ((int32_t)(ip << 16)) >> 24;
	n4 = ((int32_t)(ip << 24)) >> 24;
	if (*((uint8_t *)&x))
	{
		int	tmp;
		tmp = n4; n4 = n1; n1 = tmp;
		tmp = n3; n3 = n2; n2 = tmp;
	}
	clientIp = size2Hex(n1, 10) + '.' + size2Hex(n2, 10) + '.'\
				+ size2Hex(n3, 10) + '.' + size2Hex(n4, 10);
	return (clientIp);
}

void	RequestData::procServer()
{
	serv = Server::determineServer(_servsList, hostName);
	if (!serv)
		setHeaderState(e_serv, false);
	else
		setHeaderState(e_serv, true);
}

void	RequestData::procUri()
{
	if (!uri.procUri(*_uri))
	{
		setHeaderState(e_uri, false);
		return ;
	}
	location = Server::findLocation(serv, uri.script_name);
	if (!location)
	{
		setHeaderState(e_loc, false);
		error_code = 404;
		return ;
	}
	if (!uri.setTranslatedPath(location))
		setHeaderState(e_uri, false);
}

void		RequestData::procAuthorization()
{
	constMapIter	header;

	if (location->authLogPass.empty() || location->auth.empty())
		return ;
	if (!location)
		return (setHeaderState(e_auth, false));
	header = _reqHeads->find("authorization");
	if (header == _reqHeads->end() &&\
		!location->authLogPass.empty() &&\
		!location->auth.empty())
	{
		error_code = 401;
		return (setHeaderState(e_auth, false));
	}
	std::string	authCont = header->second;
	size_t spacePos = authCont.find(' ', 0);
	if (spacePos == std::string::npos)
		{setHeaderState(e_auth, false); return ;}
	std::string	authType = authCont.substr(0, spacePos);
	stringToLower(authType);
	if (authType != "basic")
		{setHeaderState(e_auth, false); return ;}
	std::string encodedReq = authCont.substr(spacePos + 1);
	std::string encoded = base64encode(location->authLogPass);
	if (encoded == encodedReq)
		return (setHeaderState(e_auth, true));
	else
	{
		error_code = 401;
		return (setHeaderState(e_auth, false));
	}
}

void		RequestData::addCgiVar(int i, std::string const &s)
{
	if (badalloc_index != -1)
		return ;
	cgi_conf[i] = strdup(s.c_str());
	if (!cgi_conf[i])
	{
		setHeaderState(e_cgi, false);
		badalloc_index = i;
	}
}

void			RequestData::cleanCGIenv()
{
	int i = 0;
	while (cgi_conf[i])
	{
		if (i == badalloc_index)
			break ;
		free(cgi_conf[i++]);
	}
}

void		RequestData::procCGI(size_t contLen, sockaddr_in addr)
{
	if (!serv || !location || (errorMask & e_uri))
		return (setHeaderState(e_cgi, false));
	if (location->cgi.find(uri.extension) == location->cgi.end())
		return ;
	setHeaderState(e_cgi, true);
	if (!(cgi_conf = (char **)malloc(envCgiSize * sizeof(char *))))
		setHeaderState(e_cgi, false);
	int	i = 0;
	if ((in & e_auth) && !(errorMask & e_auth))
		addCgiVar(i++, "AUTH_TYPE=Basic");
	addCgiVar(i++, "GATEWAY_INTERFACE=CGI/1.1");
	addCgiVar(i++, "SERVER_PROTOCOL=HTTP/1.1");
	addCgiVar(i++, "SERVER_SOFTWARE=BSHABILLUM/1.1");
	addCgiVar(i++, "SERVER_NAME=" + serv->serverName);
	addCgiVar(i++, "SERVER_PORT=" + size2Hex(serv->port, 10));
	if (!uri.query_string.empty())
		addCgiVar(i++, "QUERY_STRING=" + uri.query_string);
	addCgiVar(i++, "REQUEST_URI=" + uri.request_uri);
	if (!uri.path_info.empty())
	{
		addCgiVar(i++, "PATH_INFO=" + uri.path_info);
		addCgiVar(i++, "PATH_TRANSLATED=" + uri.path_translated);
	}
	addCgiVar(i++, "SCRIPT_NAME=" + uri.script_name);
	if ((in & e_contType) && !(errorMask & e_contType))
		addCgiVar(i++, "CONTENT_TYPE=" + contentType["type"]);
	if (contLen > 0)
		addCgiVar(i++, "CONTENT_LENGTH=" + size2Hex(contLen, 10));
	addCgiVar(i++, "REMOTE_ADDR=" + getClientIp(addr.sin_addr.s_addr));
	addCgiVar(i++, "REQUEST_METHOD=" + *_method);
	if ((in & e_auth) && !(errorMask & e_auth))
	{
		std::string	username = location->authLogPass.substr(0,\
								location->authLogPass.find_first_of(':'));
		addCgiVar(i++, "REMOTE_IDENT=" + username + '.' + hostName);
		addCgiVar(i++, "REMOTE_USER=" + username);
	}
	cgi_conf[i] = NULL;
	if (badalloc_index != -1)
		{cleanCGIenv(); return ;}
	for (int k = 0; k < i; k++)
		std::cout << cgi_conf[k] << std::endl;
}

void		RequestData::prepareData(size_t contLen, sockaddr_in addr)
{
	procHost();
	procQualityHeaders();
	procUserAgent();
	procContentType();
	procServer();
	procUri();
	procAuthorization();
	procCGI(contLen, addr);
	std::cout << "LOCATION_ROOT: " << location->root << std::endl;
	// std::cout << "CGI_ROOT: " << location->cgi.find(uri.extension) << std::endl;

}
