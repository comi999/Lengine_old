#include <iostream>
#include <mutex>

#include "CGE.hpp"

void main()
{
	std::cout << "Hello World!" << std::endl;
	int i = 0;

	while(1)
	{
		std::cout << "STEP: " << i << std::endl;
	}
}