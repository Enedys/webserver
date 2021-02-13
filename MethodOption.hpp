#include "include_resources.hpp"
#include "Parser.hpp"

class MethodOption : public AMethod
{
private:
    /* data */
    MethodOption(/* args */);
public:
	MethodOption(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodOption();
};

MethodOption::~MethodOption()
{
}

