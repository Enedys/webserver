#include "includes.hpp"

class MethodOption : public AMethod
{
private:
    /* data */
    MethodOption(/* args */);
public:
	MethodOption(const serv_config *config, Request const *req) : AMethod(config, req) {};
    ~MethodOption();
};

MethodOption::~MethodOption()
{
}

