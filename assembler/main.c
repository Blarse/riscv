#include <stdio.h>

#include "common.c"

typedef struct

void pass1(FILE* InputFile)
{


}


int main(int ArgsCount, char **Args)
{
	if(ArgsCount != 3)
		panic("Wrong arguments\nUsage: %s <input-file> <output-file>", Args[0]);

	char *InputFileName = Args[1];
	char *OutputFileName = Args[2];

	FILE *InputFile = fopen(InputFileName, "r");
	if(!InputFile)
		panic("Can't open '%s' for reading", InputFileName);

	FILE *OutputFile = fopen(OutputFileName, "w");
	if(!OutputFile)
		panic("Can't create '%s'", OutputFileName);

	pass1(InputFile);

	return 0;
}
