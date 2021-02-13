#pragma once
#include "include_resources.hpp"
#include "Parser.hpp"

class MethodHead: public AMethod
{
private:
    /* data */
    MethodHead(/* args */);
public:
	MethodHead(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodHead();
};

MethodHead::~MethodHead()
{
}
