#include "includes.hpp"

class MethodPut: public AMethod
{
private:
    /* data */
    MethodPut(/* args */);
public:
	MethodPut(const serv_config *config, Request const *req) : AMethod(config, req) {};
    ~MethodPut();
};

MethodPut::~MethodPut()
{
}
