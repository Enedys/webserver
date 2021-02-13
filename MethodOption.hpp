#include "includes.hpp"

class MethodOption : public AMethod
{
private:
    /* data */
    MethodOption(/* args */);
public:
	MethodOption(const serv_config *config) : AMethod(config) {};
    ~MethodOption();
};

MethodOption::MethodOption(/* args */)
{
}

MethodOption::~MethodOption()
{
}

