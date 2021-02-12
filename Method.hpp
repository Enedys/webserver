#pragma once
#include "includes.hpp"

class AMethod
{
private:
	stringMap	_headersMap;
	int			_status;
public:
	AMethod(/* args */);
	virtual int		createHeader();
	virtual int		createErrorHeader();
	virtual int		sendHeader();
	virtual int		readBody();
	virtual int		processRequest(std::string	const &location);
	virtual int		sendBody();
	virtual ~AMethod() = 0;
};

AMethod::AMethod()
{
}

AMethod::~AMethod()
{
}
