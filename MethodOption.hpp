#pragma once
#include "Method.hpp"

class MethodOption : public AMethod
{
private:
	MethodOption();
	MethodOption(MethodOption const & src);
	MethodOption & operator=(MethodOption const & src);

public:
	MethodOption(int &status, RequestData &data) :
		AMethod(status, data) {_type = OPTION; };
	~MethodOption();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	manageRequest();
};
