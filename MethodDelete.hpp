#pragma once
#include <dirent.h>
#include "Method.hpp"

class MethodDelete : public AMethod
{
private:
	MethodDelete();
	MethodDelete(MethodDelete const & src);
	MethodDelete & operator=(MethodDelete const & src);

public:
	MethodDelete(int &status, RequestData &data) :
		AMethod(status, data) { _type = DELETE; };
	~MethodDelete();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	manageRequest();

	int						deleteDirectory(std::string const &path);
};
