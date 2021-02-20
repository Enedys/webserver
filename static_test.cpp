#include <iostream>

int foo()
{
	static int foos = 5;
	std::cout << "Static var inside foo: " << foos << std::endl;
	foos++;
	return (foos);
}

int main(int argc, char **argv)
{
	for (int i = 0; i < 10; i++)
		foo();
	return (0);
}
