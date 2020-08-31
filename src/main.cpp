#include "c8asm.h"

int main()
{

	c8asm* hello = new c8asm("test/test.asm");

	hello->parse();
	
	return 0;
}