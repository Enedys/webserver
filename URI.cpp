#include "URI.hpp"

URI::URI() {};
URI::~URI() {};

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
		return (c - 'a');
	else if (c >= 65 && c <= 90)	// uppercase ascii
		return (c - 'A');
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
	int	validSym;
	int	i = 0;
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
	while (env.second != std::string::npos && env.second >= 0)
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
	size_t	root_len = locs->root.length();
	std::string	root_name;
	if (locs->root.back() == '/')
		root_name = locs->root.substr(0, root_len - 1);
	else
		root_name = locs->root;
	script_name = root_name + script_name;
	if (!path_info.empty())
		path_translated = root_name + path_info;
	return (true);
}

void		URI::cleanData()
{
	request_uri = "";
	raw_path = "";
	query_string = "";
	fragment_string = "";
	script_name = "";
	path_info = "";
	path_translated = "";
	extension = "";
	queryEnv.clear();
}
