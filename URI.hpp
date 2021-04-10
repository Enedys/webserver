#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"
class URI
{

enum	
{
	okQueryCh = 1,
	percent = '%',
	equal = '='
};

private:
	std::string			normalizeURI(std::string const &s);
	std::pair<std::string, std::string>	
						parseRawPath(std::string const &s);
	std::pair<std::string, int>
						getEnvVar(std::string const &s, size_t start);
	int					isHex(int c);
	bool				isValidPath();
	int					isValidQuerySymbol(int c);

public:
	std::string			request_uri;
	std::string			raw_path;
	std::string			query_string;
	std::string			fragment_string;
	std::string			script_name;
	std::string			path_info;
	std::string			path_translated;
	std::string			extension;
	std::vector<std::string>
						queryEnv;


	bool			procUri(std::string const &uri);
	bool			setTranslatedPath(s_loc const *locs);
	bool			setTranslatedPath(std::string const &root);
	bool			getEnvVarVector();
	void			cleanData();

	URI				&operator=(URI const &other);
	URI();
	~URI();
};
