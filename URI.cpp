#include "URI.hpp"

URI::URI() {}
URI::~URI() {}
URI				&URI::operator=(URI const &other)
{
	if (this == &other)
		return (*this);
	request_uri = other.request_uri;
	raw_path = other.raw_path;
	query_string = other.query_string;
	fragment_string = other.fragment_string;
	script_name = other.script_name;
	path_info = other.path_info;
	path_translated = other.path_translated;
	extension = other.extension;
	queryEnv = other.queryEnv;
	return (*this);
}


int			URI::isValidQuerySymbol(int c)
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

int			URI::isHex(int c)
{
	if (c >= 47 && c <= 57)			// digit
		return (c - '0');
	else if (c >= 97 && c <= 122)	// lowercase ascii
		return (c - 'a' + 10);
	else if (c >= 65 && c <= 90)	// uppercase ascii
		return (c - 'A' + 10);
	return (-1);
}

bool		URI::isValidPath()
{
	return (true);
}

std::pair<std::string, int>
			URI::getEnvVar(std::string const &s, size_t start)
{
	std::pair<std::string, int> res;
	res.second = -1;
	size_t	eqPos = s.find('=', start);
	size_t	delimPos = query_string.find_first_of(";&", eqPos);
	if (delimPos == std::string::npos)
		delimPos = s.length();
	std::string	envVar = "HTTP_";
	envVar.reserve(5 + delimPos - start);
	size_t	i = 0;
	int	eqNum = 0;
	while (i < delimPos - start)
	{
		int	c = isValidQuerySymbol(s[i]);
		if (c == okQueryCh)
			envVar.push_back(s[i]);
		else if (c == equal)
		{
			if (++eqNum == 1)
				envVar.push_back(s[i]);
		}
		else
		{
			res.second = -2;
			return (res);
		}
		i++;
	}
	res.first = envVar;
	if (eqPos != std::string::npos)
		res.second = delimPos;
	return (res);
}

bool		URI::getEnvVarVector()
{
	std::pair<std::string, int>	env = getEnvVar(query_string, 0);
	while ((size_t)env.second != std::string::npos && env.second >= 0)
	{
		queryEnv.push_back(env.first);
		env = getEnvVar(query_string, env.second + 1);
	}
	if (env.second == -2)
		return (false);
	return (true);
}

std::string	URI::normalizeURI(std::string const &s)
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
		if (s[i] == '%')
		{
			if (s.length() - i <= 2)
				return ("");
			int	c1 = isHex(s[i + 1]);
			int	c2 = isHex(s[i + 2]);
			if (c1 != -1 && c2 != -1)
			{
				normUri.push_back(c1 * 16 + c2);
				i += 2;
			}
			else
				return ("");
		}
		else
			normUri.push_back(s[i]);
		i++;
	}
	return (normUri);
}

std::pair<std::string, std::string>
			URI::parseRawPath(std::string const &s)
{
	std::pair<std::string, std::string>	script_info;
	size_t	firstDot = s.find_first_of('.');
	if (firstDot == std::string::npos)
	{
		script_info.first = s.substr(0, s.length());
		return (script_info);
	}
	size_t	slashAfterDot = s.find_first_of('/', firstDot);
	script_info.first = s.substr(0, slashAfterDot);
	if (slashAfterDot != std::string::npos)
		script_info.second = s.substr(slashAfterDot);
	return (script_info);
}

bool		URI::procUri(std::string const &uri)
{
	request_uri = normalizeURI(uri);
	if (request_uri == "")
		return (false);
	size_t	queryPos = request_uri.find('?');
	raw_path = request_uri.substr(0, queryPos);
	if (!isValidPath())
		return (false);
	if (queryPos != std::string::npos)
	{
		size_t	fragmentPos = request_uri.find('#', queryPos);
		query_string = request_uri.substr(queryPos + 1, \
								fragmentPos - queryPos - 1);
		if (fragmentPos != std::string::npos)
			fragment_string = request_uri.substr(fragmentPos);
	}
	std::pair<std::string, std::string>
			script_info = parseRawPath(raw_path);
	script_name = script_info.first;
	path_info = script_info.second;
	size_t	extPos = script_name.find_last_of('.');
	if (extPos != std::string::npos)
		extension = script_name.substr(extPos + 1);
	return (true);
}



bool		URI::setTranslatedPath(s_loc const *locs)
{
	if (!locs)
		return (false);
	size_t	root_len = locs->root.size();
	std::string	root_name;
	if (root_len > 0)
	{
		if (locs->root[root_len - 1] == '/')
			root_name = locs->root.substr(0, root_len - 1);
		else
			root_name = locs->root;
	}
	#ifdef TESTER
	size_t	lock_pos = script_name.find(locs->path);
	if (lock_pos == std::string::npos)
		return (false);
	script_name = script_name.substr(lock_pos + locs->path.size());
	if (!script_name.empty())
		if (script_name[0] != '/')
			script_name = "/" + script_name;
	#endif
	script_name = root_name + script_name;
	if (!path_info.empty())
		path_translated = root_name + path_info;
	return (true);
}

bool		URI::setTranslatedPath(std::string const &root)
{
	size_t	root_len = root.length();
	std::string	root_name;
	if (root_len > 0)
	{
		if (root[root_len - 1] == '/')
			root_name = root.substr(0, root_len - 1);
		else
			root_name = root;
	}
	script_name = root_name + script_name;
	if (!path_info.empty())
		path_translated = root_name + path_info;
	return (true);
}

void		URI::cleanData()
{
	request_uri.clear();
	raw_path.clear();
	query_string.clear();
	fragment_string.clear();
	script_name.clear();
	path_info.clear();
	path_translated.clear();
	extension.clear();
	queryEnv.clear();
}
