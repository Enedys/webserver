#pragma once
#include "includes.hpp"


class MethodHead: public AMethod
{
private:
    /* data */
    MethodHead(/* args */);
public:
	MethodHead(const serv_config *config, Request const *req) : AMethod(config, req) {};
    ~MethodHead();
};

MethodHead::~MethodHead()
{
}
