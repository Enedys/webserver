#include "include_resources.hpp"
#include "Parser.hpp"

typedef std::map<std::string, int>	s2IntMap;
typedef std::pair<bool, s2IntMap>	qualityMap;
typedef std::pair<bool, stringMap>	contTypeMap;


class RequestData
{
	private:
		t_ext_serv const	&servs;
		stringMap const		&reqHeads;
		RequestData();

		bool			isValidHost(std::string const &s, size_t port);
		bool			isValidUserAgent(std::string const &s);
		contTypeMap		getContentType(std::string const &s);
		qualityMap		parseAcceptionLine(std::string const &s, int isLang, int isQ);
		std::string		base64encode(std::string const &s);
		std::string		base64decode(std::string const &s);
	
	
	public:
		RequestData(t_ext_serv const &s, stringMap const &rHs);
		enum
		{
			host = 1,
			accChSet = 1 << 1,
			accLang = 1 << 2,
			contLang = 1 << 3,
			contType = 1 << 4,
			auth = 1 << 5,
			userAgent = 1 << 6,
			referer = 1 << 7,
		};
		t_serv		*serv;
		std::string	hostName;
		std::string	authDecode;
		s2IntMap	acceptCharset;
		s2IntMap	acceptLanguage;
		s2IntMap	contentLanguage;
		stringMap	contentType;
		int			errorMask;
		int			in;

		void		prepareData();

};