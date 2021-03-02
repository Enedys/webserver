#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

typedef std::map<std::string, std::string> stringMap;
typedef std::map<std::string, int>	s2IntMap;
typedef std::pair<bool, s2IntMap>	qualityMap;
typedef std::pair<bool, stringMap>	contTypeMap;

contTypeMap	getContentType(std::string const &s)
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

int main()
{
	std::vector<std::string> v = 
	{
		"text/html; charset=utf-8",
		"multipart/form-data; boundary=something; charset=utf-8",
		"charset=utf-8",
		"text/html",
		"text/html; ",
		"multipart/form-data; boundary=-----asd-a-d;",
		""
	};
	for (int i = 0; i < v.size(); i++)
	{
		contTypeMap m = getContentType(v[i]);
		std::cout << "\033[35m" << m.first << "\t\033[32m" << v[i] << "\033[0m\n";
		std::cout << "type: " << m.second["type"] << std::endl;
		std::cout << "charset: " << m.second["charset"] << std::endl;
		std::cout << "boundary: " << m.second["boundary"] << std::endl;
	}
	return (0);
}