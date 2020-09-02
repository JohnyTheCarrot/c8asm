#include "c8asm.h"

int main(int argc, char* argv[])
{
	
	if (argc < 2){
		printf("no input.\nUsage: %s input output\n", argv[0]);
		return 1;
	}
	c8asm* assembler = new c8asm(argv[1]);

	

	const char* output_filename = argv[2];

	assembler->parse();
	assembler->save(output_filename);
	
	return 0;
}