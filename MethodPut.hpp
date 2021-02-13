#include "includes.hpp"

class MethodPut: public AMethod
{
private:
    /* data */
    MethodPut(/* args */);
public:
	MethodPut(const serv_config *config) : AMethod(config) {};
    ~MethodPut();
};

MethodPut::~MethodPut()
{
}
