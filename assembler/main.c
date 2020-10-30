#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "common.c"

#define LINE_BUFFER_SIZE 256

typedef struct
{
	uint32 Value;
	char Name[32];
} symbol;

int isempty(const char *String)
{
	while(*String != '\0')
	{
		if(!isspace(*String))
			return 0;
		String++;
	}
	return 1;
}


char * firstchar(const char *String)
{
	while(*String != '\0')
	{
		if(!isspace(*String))
			return String;
		String++;
	}
	return NULL;
}

void pass1(FILE* InputFile)
{
	char LineBuffer[LINE_BUFFER_SIZE];
	char *LinePtr;
	uint32 LineNum = 0;
	size_t LineLength = 0;
	uint32 Result = 0;


	uint32 LocationCounter;
	char *Lable, *Mnemonic, *Operands, *Comment;


	for(;;)
	{
		++LineNum;

		LinePtr = fgets(LineBuffer, LINE_BUFFER_SIZE, InputFile);
		if(!LinePtr)
		{
			if(feof(InputFile))
				break;
			panic("[%s] file parsing error",__func__);
		}

		if(isempty(LineBuffer))
			continue;

		LineLength = strlen(LineBuffer);

		if(LineBuffer[LineLength-1] != '\n')
			panic("The line %d is too long!", LineNum);

		LineBuffer[LineLength-1] = '\0';



		LinePtr = firstchar(LineBuffer);
		if(*LinePtr == '.')
		{
			//directive

		}
		else
		{
			//check for a lable
		}


		printf("%d: '%s'\n", LineNum, LineBuffer);
	}

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
