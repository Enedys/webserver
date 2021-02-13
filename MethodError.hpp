#include "includes.hpp"

class MethodError : public AMethod
{
private:
    /* data */
    MethodError(/* args */);
public:
	MethodError(const serv_config *config) : AMethod(config) {};
    ~MethodError();
};

MethodError::MethodError(/* args */)
{
}

MethodError::~MethodError()
{
}
