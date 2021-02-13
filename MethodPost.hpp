#include "include_resources.hpp"
#include "Parser.hpp"

class MethodPost: public AMethod
{
private:
    /* data */
    MethodPost(/* args */);
public:
	MethodPost(const t_serv *config, Request const *req) : AMethod(config, req) {};
    ~MethodPost();
};

MethodPost::~MethodPost()
{
}
