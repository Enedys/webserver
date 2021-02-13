#include <iostream>
int main()
{
	int a = 5;
	int	v = 6;
	int   * const  s = &a;
	std::cout << *s << std::endl;
	// s = &v;
	*s = 3;
	std::cout << *s << std::endl;
	return (0);
}