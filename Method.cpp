#include "Method.hpp"

// AMethod::AMethod() {};
AMethod::~AMethod() {};

void	AMethod::mapToString(stringMap const &headersMap, std::string *output){
	*output += "HTTP/1.1 200 OK\r\n";
	for (constMapIter it = headersMap.begin(); it != headersMap.end(); ++it)
		*output += (it->first) + ": " + (it->second) + CRLF;
	*output += CRLF;
	std::cout << "READY_HEADER: " << *output << std::endl;
}
