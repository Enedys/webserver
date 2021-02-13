#include "includes.hpp"

class MethodPost: public AMethod
{
private:
    /* data */
    MethodPost(/* args */);
public:
	MethodPost(const serv_config *config) : AMethod(config) {};
    ~MethodPost();
};

MethodPost::MethodPost(/* args */)
{
}

MethodPost::~MethodPost()
{
}
