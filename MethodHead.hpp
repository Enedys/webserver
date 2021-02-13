#pragma once
#include "includes.hpp"


class MethodHead: public AMethod
{
private:
    /* data */
    MethodHead(/* args */);
public:
	MethodHead(const serv_config *config) : AMethod(config) {};
    ~MethodHead();
};

MethodHead::MethodHead(/* args */)
{
}

MethodHead::~MethodHead()
{
}
