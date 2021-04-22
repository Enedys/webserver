#pragma once
#include "Method.hpp"

class MethodPut : public AMethod
{
private:
	MethodPut();
	MethodPut(MethodPut const & src);
	MethodPut & operator=(MethodPut const & src);

public:
	MethodPut(int &status, RequestData &data) :
		AMethod(status, data) { _type = PUT; };
	~MethodPut();
	virtual MethodStatus	processBody(const std::string &requestBody, MethodStatus bodyStatus);
	virtual MethodStatus	manageRequest();
	void					setUploadPath();
	MethodStatus			prepareFdToWrite();
};
