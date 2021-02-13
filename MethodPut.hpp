#include "include_resources.hpp"
#include "Parser.hpp"

class MethodPut: public AMethod
{
private:
    /* data */
    MethodPut(/* args */);
public:
	MethodPut(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodPut();
};

MethodPut::~MethodPut()
{
}
