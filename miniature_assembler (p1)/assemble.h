//Mohammad Amin Kiani 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> /*In the C90 standard, there was no such thing as bool, and int could be used instead of bool or an enum could be defined.
In the C99 standard, the _Bool type was added, which used bool by connecting the stdbool.h header file.
But in C++, a separate standard data type named bool is defined, which is one of the keywords of the language. This is the difference between the Boolean data type in these two languages.*/

//the number of R, I and J type instructions
#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 2
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2

/* this structure is defined to hold ever entity of symbol table    *
 * which, after finding the length, will become an array of symbols */
struct symbolTable
{
    int value;
    char *symbol;
};

/* every time an instruction is read, it will be tokenized and each   *
 * token will reside in one element of this structure               */
struct instruction
{
    // 0 means r type, 1 means i type and 2 means j type
    size_t instType;
    size_t intInst;
    char *mnemonic;
    char inst[9];
    int rs;
    int rt;
    int rd;
    int imm;
    int PC;
};

void error(char *);

bool isDecimal(char* );

/* the follwing function, findSymTabLen, will scan the assembly    *
 * code and count the number of symbols and return it              */
int findSymTabLen(FILE *);

/* this function fills up the symbol table                         */
int fillSymTab(struct symbolTable *, FILE *);

/* and the following functions are other ancillary functions        */
void formInst(struct instruction *, FILE *);

int hex2int(char *);

void int2hex16(char *, int);
