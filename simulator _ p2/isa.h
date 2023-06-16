//Mohammad Amin Kiani 4003613052

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> /*In the C90 standard, there was no such thing as bool, and int could be used instead of bool or an enum could be defined.
In the C99 standard, the _Bool type was added, which used bool by connecting the stdbool.h header file.
But in C++, a separate standard data type named bool is defined, which is one of the keywords of the language. This is the difference between the Boolean data type in these two languages.*/

#define RTYPE 0
#define ITYPE 1
#define JTYPE 2

//simulator 

struct RegFile
{
    bool isUsed[16];
    int Registers[16];
};

struct InsMem
{
    int *inputList;
    int instType;
    char imm[8];
};

struct ALU
{
    bool isHalted;
};

struct DataMem
{
    int memUsed;
};

struct Adder
{
    int PC;
};

//----------------------------------------------------

void int2hex16(char *, int);

int *ReadFile(FILE *fileptr, int count);

int hex2int(char *hex);