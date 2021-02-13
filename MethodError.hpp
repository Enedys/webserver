#include "include_resources.hpp"
#include "Parser.hpp"

class MethodError : public AMethod
{
private:
    /* data */
    MethodError(/* args */);
public:
	MethodError(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodError();
};

MethodError::~MethodError()
{
}
