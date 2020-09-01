#include "c8asm.h"

int main()
{
	c8asm* assembler = new c8asm("test/aaa.asm");

	const char* output_filename = "output/output";

	assembler->parse();
	assembler->save(output_filename);
	
	return 0;
}