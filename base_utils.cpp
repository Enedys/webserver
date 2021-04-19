#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

static int	isValidCh(int c, int base)
{
	if (base <= 10)
		return (c >= '0' && c <= '9');
	return ((c >= '0' && c <= '9') || (c >= 'A' && c <= ('A' + base - 10)) || \
	(c >= 'a' && c <= ('a' + base - 10)));
}

int			string2Size(const std::string &str, int base=16)
{
	int		i = 0;
	int		nbr;

	if ((base < 2 || base > 16))
		return (-1);
	nbr = 0;
	while (str[i] && isValidCh(str[i], base))
	{
		if (str[i] >= 'A' && 'F' >= str[i])
			nbr = (nbr * base) + (str[i] - 'A' + 10);
		else if (str[i] >= 'a' && 'f' >= str[i])
			nbr = (nbr * base) + (str[i] - 'a' + 10);
		else
			nbr = (nbr * base) + (str[i] - '0');
		i++;
	}
	if (str[i] != '\0')
		return (-1);
	return (nbr);
}

std::string	size2Hex(size_t num, int base = 16)
{
	const char		*dict = "0123456789abcdef";

	if (num == 0)
		return (std::string("0"));
	std::string		res;
	while (num)
	{
		res.insert(res.begin(), dict[num % base]);
		num /= base;
	}
	return (res);
}

std::string	size2Dec(size_t num, int base = 10)
{
	const char		*dict = "0123456789";

	if (num == 0)
		return (std::string("0"));
	std::string res;
	while (num)
	{
		res.insert(res.begin(), dict[num % base]);
		num /= base;
	}
	return (res);
}

void		stringToLower(std::string &s)
{
	for (size_t i = 0; i < s.length(); i++)
		s[i] = std::tolower(s[i]);
}

std::string	getLastModifiedTime(std::string const & path)
{
	char			buf2[100];
	struct stat		stats;
	struct tm		*tm2;
	std::string		lastModified;

	if (stat(path.c_str(), &stats) == 0) {
		tm2 = gmtime(&stats.st_mtime);
		strftime(buf2, 100, "%a, %d %b %Y %H:%M:%S GMT", tm2);
		lastModified = std::string(buf2);
	}
	return lastModified;
}

bool		fileExists(std::string const &f)
{
	struct stat st;
	if (stat(f.c_str(), &st) == -1)
		return false;
	return true;
}