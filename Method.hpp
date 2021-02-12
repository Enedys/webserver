#pragma once
#include "includes.hpp"

class IMethod
{
private:
public:
	IMethod(/* args */);
	void	CreateHeader();
	void	createErrorHeader();
	void	sendHeader();
	void	sendError();
	virtual ~IMethod();
};

IMethod::IMethod(/* args */)
{
}

IMethod::~IMethod()
{
}
