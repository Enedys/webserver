#include "includes.hpp"

class MethodError : public AMethod
{
private:
    /* data */
    MethodError(/* args */);
public:
	MethodError(const serv_config *config, Request const *req) : AMethod(config, req) {};
    ~MethodError();
};

MethodError::~MethodError()
{
}
