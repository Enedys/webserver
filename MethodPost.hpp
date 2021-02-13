#include "includes.hpp"

class MethodPost: public AMethod
{
private:
    /* data */
    MethodPost(/* args */);
public:
	MethodPost(const serv_config *config, Request const *req) : AMethod(config, req) {};
    ~MethodPost();
};

MethodPost::~MethodPost()
{
}
