#include <string>

static int		isValidCh(int c, int base)
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
	int		sign;

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