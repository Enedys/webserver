#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"
#include "URI.hpp"
#include "Server.hpp"

#define envCgiSize 20

typedef std::map<std::string, int>			s2IntMap;
typedef std::pair<bool, s2IntMap>			qualityMap;
typedef std::pair<bool, stringMap>			contTypeMap;

class RequestData
{	
	public:
		RequestData(t_ext_serv const *s, stringMap const *rHs, stringMap const *rFl);
		RequestData();
		~RequestData();
		typedef enum
		{
			e_host = 1,
			e_accChSet = 1 << 1,
			e_accLang = 1 << 2,
			e_contLang = 1 << 3,
			e_contType = 1 << 4,
			e_userAgent = 1 << 5,
			e_referer = 1 << 6,
			e_auth = 1 << 7,
			e_serv = 1 << 8,
			e_uri = 1 << 9,
			e_loc = 1 << 10,
			e_cgi = 1 << 11,
		}		headerNum;

		std::string		hostName;
		URI				uri;
		t_serv const	*serv;
		s_loc const		*location;
		char			**cgi_conf;
		s2IntMap		acceptCharset;
		s2IntMap		acceptLanguage;
		s2IntMap		contentLanguage;
		stringMap		contentType;

		int				in;
		int				errorMask;
		int				error_code;
		void			prepareData(size_t contLen, sockaddr_in addr);
		void			cleanData();
		void			setData(t_ext_serv const *s, stringMap const *rHs,\
													stringMap const *rFl);
	
	private:
		t_ext_serv const	*_servsList;
		stringMap const		*_reqHeads;
		std::string	const	*_uri;
		std::string	const	*_method;
		int					badalloc_index;

		bool			isValidHost(std::string const &s, size_t port);
		bool			isValidUserAgent(std::string const &s);
		contTypeMap		getContentType(std::string const &s);
		qualityMap		parseAcceptionLine(std::string const &s, int isLang, int isQ);
		std::string		base64encode(std::string const &s);
		std::string		base64decode(std::string const &s);

		void			setHeaderState(headerNum hN, bool error);
		void			procHost();
		void			procQualityHeaders();
		void			procUserAgent();
		void			procContentType();
		void			procServer();
		void			procUri();
		void			procAuthorization();
		void			procCGI(size_t contLen, sockaddr_in addr);
		void			addCgiVar(int i, std::string const &s);
		void			cleanCGIenv();
};