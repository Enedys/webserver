#ifndef MethodGet_HPP
# define MethodGet_HPP

# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>

# include "Method.hpp"

class AMethod;
// class Header;
# define BUFSIZE 4096

//max body size

// class MethodGet : public AMethod, public Header
class MethodGet : public AMethod
{
private:
	MethodGet(MethodGet const & src);
	MethodGet & operator=(MethodGet const & src);
	MethodGet();

public:
	MethodGet(t_serv const &config, int &code, stringMap const &headersMapRequest);
	~MethodGet();

	virtual MethodStatus	readRequestBody();
	virtual MethodStatus	manageRequest(std::string const &path);
	virtual MethodStatus	createHeader();
	virtual MethodStatus	sendHeader(int socket);
	virtual MethodStatus	sendBody(int socket);
};

#endif
