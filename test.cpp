#include "includes.hpp"
#include <string>
#include <map>

int main()
{
   std::map<std::string, std::string> map;
   map["hello"] = "world";
   std::map<std::string, std::string>::iterator it;
   it = map.begin();
   std::cout << "Begin: " << it->first.length() << std::endl;
   it = map.end();
   std::cout << "End: " << it->first.length() << std::endl;
}