#pragma
#include <string>
#include <map>
#include <ctype.h>

typedef std::pair<bool, std::map<std::string, int> >	qualityMap;
class HeaderAnalyser
{
public:
	static bool				isValidHost(std::string const &s);
	static bool				isValidUserAgent(std::string const &s);
	static qualityMap		parseAcceptionLine(std::string const &s);
	static std::string		base64encode(std::string const &s);
	static std::string		base64decode(std::string const &s);
	HeaderAnalyser();
	~HeaderAnalyser();
};
