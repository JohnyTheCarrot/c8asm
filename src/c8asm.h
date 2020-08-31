#include<stdio.h>
#include<stdlib.h>

class c8asm{
private:
	FILE* fp;
	int fsize;
	char* fmem;
public:
	c8asm(const char* str){
		if (!(fp = fopen(str, "r"))){
			fprintf(stderr, "failed to open %s.\n", str);
			exit(1);
		}
		//file size
		fseek(fp, 0l, SEEK_END);
		fsize = ftell(fp);
		fseek(fp, 0l, SEEK_SET);

		//allocate memory
		fmem = (char*)malloc(sizeof(char) * fsize);
		for (int i = 0; i < fsize; i++){
			fmem[i] = fgetc(fp);
		}
		fclose(fp);
	}
	~c8asm(){
		free(fmem);
	}

	void parse(){
		for (int i = 0; i < fsize; i++) printf("%c", fmem[i]);
	}

};