#include "include_resources.hpp"
#include "Parser.hpp"

typedef std::map<std::string, int>			s2IntMap;
typedef std::pair<bool, s2IntMap>			qualityMap;
typedef std::pair<bool, stringMap>			contTypeMap;
typedef std::vector<s_loc>::iterator		locIter;
typedef std::vector<s_loc>::const_iterator	constLocIter;

enum	
{
	okQueryCh = 1,
	percent = '%',
	equal = '=',
};

typedef struct	s_cgi_conf
{

}				t_cgi_conf;

class RequestData
{	
	public:
		RequestData(t_ext_serv const &s, stringMap const &rHs, std::string const &uri);
		typedef enum
		{
			host = 1,
			accChSet = 1 << 1,
			accLang = 1 << 2,
			contLang = 1 << 3,
			contType = 1 << 4,
			auth = 1 << 5,
			userAgent = 1 << 6,
			referer = 1 << 7,
			path = 1 << 8,
			query = 1 << 9,
			servT = 1 << 10,
			locFind = 1 << 11,
		}		headerNum;

		t_serv const	*serv;
		s_loc const		*location;
		t_cgi_conf		cgi_conf;
		std::string		hostName;
		std::string		pathFromUri;
		std::string		pathToFile;
		std::string		queryUri;
		std::string		fragmentUri;
		std::vector<std::string>	
						queryEnv;

		std::string		authDecode;
		s2IntMap		acceptCharset;
		s2IntMap		acceptLanguage;
		s2IntMap		contentLanguage;
		stringMap		contentType;

		int				errorMask;
		int				in;
		void			prepareData();
	
	
	private:
		t_ext_serv const	&servsList;
		stringMap const		&reqHeads;
		std::string	const	&uri;
		RequestData();

		bool			isValidHost(std::string const &s, size_t port);
		bool			isValidUserAgent(std::string const &s);
		contTypeMap		getContentType(std::string const &s);
		qualityMap		parseAcceptionLine(std::string const &s, int isLang, int isQ);
		std::string		base64encode(std::string const &s);
		std::string		base64decode(std::string const &s);

		void			setHeaderState(headerNum hN, bool error);
		void			procQualityHeaders();
		void			procUserAgent();
		void			procContentType();
		void			procHost();
		bool			isValidPath();
		void			uriParse(std::string const &uri, bool envNeed);
		void			determineServer();
		bool			findLocation();
		void			procAuthorization();
		void			getCGIconfig();
		std::pair<std::string, int>		getEnvVar(std::string const &s, size_t start);
};