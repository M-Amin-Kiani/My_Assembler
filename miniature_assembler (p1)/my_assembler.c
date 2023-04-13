//Mohammad Amin Kiani 

#include "assemble.h" //Functions and structures required by the program


/*argc is a positive numeric variable and counts the number of parameters passed to the program.
 *  By default, it has a value of 1.
argv is a string variable in the form of an array.
The first value is the program name. This value is the same as argv[0].
The values from the second to the next are the values.*/
void main(int argc, char **argv)  // argc + argv + env
{

    FILE *assemblefilep, *machinefilep, *fopen();// pointers to the program's input and output files.

    struct symbolTable *pSymTab;
    int symTabLen;
    int i = 0, j = 0, found, noInsts;

    struct instruction *currInst = (struct instruction *)malloc(sizeof(struct instruction));

    int lineSize = 100; // the maximum length of a text line in the input file.

    char line[50]; // used to read the lines of the input file.

    char *token, *token2; // used to separate words from a command in the input file.

    char *instructions[]={"add", "sub", "slt", "or", "nand"
                          , "addi" , "slti", "ori", "lui", "lw"
                          , "sw" , "beq", "jalr", "j", "halt"};  // 15 basic instructions.

    int instCnt = 0;
    char hexTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7'
                         , '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}; //table for converting 10 to 16 .

    char lower[5]; // used to convert uppercase letters to lowercase letters in the getInstType() function.

    /*If the number of parameters is less than 3, it means
     that the name of the program file and the name of the input file are not entered as parameters
     the program terminates with error code 1.
     If the number of parameters is correct, the program continues and places the input parameters
     such as the name of the program file and
     the name of the input file in the variables argv[0] and argv[1].*/
    if(argc < 3)
    {
        printf("***** Please run this program as follows: \n");
        printf("***** %s assprog.as machprog.m\n",argv[0]);
        printf("***** where assprog.as is your assembly program\n");
        printf("***** and machprog.m will be your machine code.\n");  // .m for matlab so I used .mc
        exit(1);
    }

    if((assemblefilep=fopen(argv[1],"r")) == NULL)  //file_input = assprog.as --> argv[1]
    {
        printf("%s cannot be opened\n", argv[1]);
        exit(1);
    }

    if((machinefilep=fopen(argv[2],"w+")) == NULL) //file_output = write and read mode ("w+")  - machprog.mc --> argv[2]
    {
        printf("%s cannot be opened\n", argv[2]);
        exit(1);
    }

    symTabLen = findSymTabLen(assemblefilep);

    pSymTab = (struct symbolTable *)malloc(symTabLen*sizeof(struct symbolTable));

    for(i=0; i <symTabLen; i++)
    {
        pSymTab[i].symbol = (char*)malloc(7);
    }

    noInsts = fillSymTab(pSymTab, assemblefilep);

    for ( i = 0; i < symTabLen - 1; i++)
    {
        for ( j = i+1; j < symTabLen; j++)
        {
            if(strcmp(pSymTab[i].symbol, pSymTab[j].symbol) == 0)
            {
                error("Multiple use of single lable"); // lables are unique
            }
        }

    }

    currInst->mnemonic =(char *)malloc(7);
    long result;
    char *temp = (char *)malloc(9);
    while(fgets(line, lineSize, assemblefilep))
    {
        currInst->PC = instCnt;

        for ( i = 0; i < 8; i++)
        {
            currInst->inst[i] = '0';  /*hold instructions in binary form.
            Each time a new line is read, the values of all members of inst are initialized to '0'.*/
        }

        instCnt++;
        found = 0;
        token = strtok(line, "\t, \n");

        for(i=0;i<symTabLen; i++)
        {
            if(strcmp(pSymTab[i].symbol, token) == 0)
            {
                token = strtok(NULL,"\t, \n");  /* must specify NULL as the first argument,
                which tells the function to continue tokenizing the string you passed in first.*/
            }
        }

        strcpy(currInst->mnemonic,token);

        currInst->instType = -1;

        if(strcmp(currInst->mnemonic, ".fill") == 0)
        {
            token = strtok(NULL, "\t, \n");
            if(isDecimal(token))
            {
                fprintf(machinefilep, "%i\n", atoi(token));  // atoi : converts str to integer
            }
            else
            {
                for ( i = 0; i < symTabLen; i++)
                {
                    if(strcmp(pSymTab[i].symbol, token) == 0)
                    {
                        currInst->imm = pSymTab[i].value;
                        break;
                    }
                }

                if(symTabLen == i)
                {
                    error("Invalid Lable");   // not found
                }

                int2hex16(temp, currInst->imm);

                currInst->inst[0] = '0';
                currInst->inst[1] = '0';
                currInst->inst[2] = '0';
                currInst->inst[3] = '0';
                currInst->inst[4] = *(temp+0);
                currInst->inst[5] = *(temp+1);
                currInst->inst[6] = *(temp+2);
                currInst->inst[7] = *(temp+3);

                result = hex2int(currInst->inst);

                fprintf(machinefilep, "%i\n", result);
            }

            continue;
        }

        for (i = 0; i < 15; i++)
        {
            if (strcmp(currInst->mnemonic, instructions[i]) == 0)
            {
                currInst->intInst = i;
                break;
            }
        }

        if(i == 15)
        {
            error("Invalid opcode");
        }

        if(i < 5) //R-Type
        {
            currInst->rd = atoi(strtok(NULL, "\t, \n"));
            currInst->rs = atoi(strtok(NULL, "\t, \n"));
            currInst->rt = atoi(strtok(NULL, "\t, \n"));
            currInst->instType = RTYPE;  //0
        }
        else if (i < 13) //I-Type
        {
            currInst->rd = 0;
            currInst->rt = atoi(strtok(NULL, "\t, \n"));
            currInst->instType = ITYPE;   //1

            if(strcmp(token, "lui") == 0 )
            {
                currInst->rs = 0;  // lui $rt, imm
            }
            else
                currInst->rs = atoi(strtok(NULL, "\t, \n"));

            token2 = strtok(NULL, "\t, \n");

            if(strcmp(token, "jalr") == 0 )
            {
                currInst->imm = 0;
            }
            else if (isDecimal(token2))
            {
                currInst->imm = atoi(token2);
            }
            else
            {
                for ( i = 0; i < symTabLen; i++)
                {
                    if(strcmp(pSymTab[i].symbol, token2) == 0)
                    {
                        currInst->imm = pSymTab[i].value;
                        break;
                    }
                }
                if(symTabLen == i)
                {
                    error("Invalid Lable");
                }
            }


            if (strcmp(token,"beq") == 0)
            {
                int temp;
                currInst->imm = currInst->imm - currInst->PC - 1;
                temp = currInst->rs;
                currInst->rs = currInst->rt;
                currInst->rt = temp;
            }
        }
        else //J-Type
        {
            currInst->instType = JTYPE;  //2
            currInst->rd = 0;
            currInst->rt = 0;
            currInst->rs = 0;
            if(strcmp(token, "j") == 0 )
            {
                token = strtok(NULL, "\t, \n");
                for ( i = 0; i < symTabLen; i++)
                {
                    if(strcmp(pSymTab[i].symbol, token) == 0)
                    {
                        currInst->imm = pSymTab[i].value;
                        break;
                    }
                }
                if(symTabLen == i)
                {
                    error("Invalid Lable");
                }
            }
            else
            {
                currInst->imm = 0;
            }
        }

        currInst->inst[1] = hexTable[currInst->intInst];

        if(currInst->imm > 32767 || currInst->imm < -32768)
        {
            error("Invalid offset");
        }

        switch (currInst->instType)
        {
        case RTYPE:
            currInst->inst[2] = hexTable[currInst->rs];
            currInst->inst[3] = hexTable[currInst->rt];
            currInst->inst[4] = hexTable[currInst->rd];
            break;

        case ITYPE:
            currInst->inst[2] = hexTable[currInst->rs];
            currInst->inst[3] = hexTable[currInst->rt];
            int2hex16(temp, currInst->imm);
            currInst->inst[4] = *temp;
            currInst->inst[5] = *(temp+1);
            currInst->inst[6] = *(temp+2);
            currInst->inst[7] = *(temp+3);
            break;

        default:
            int2hex16(temp, currInst->imm);
            currInst->inst[5] = *(temp+1);
            currInst->inst[6] = *(temp+2);
            currInst->inst[7] = *(temp+3);
            break;
        }
        currInst->inst[8] = '\0';
        result = hex2int(currInst->inst);
        fprintf(machinefilep, "%i\n", result);
    }

    //-----------------------clean
    free(temp);
    fclose(machinefilep);
    fclose(assemblefilep);
    free(pSymTab);
    free(currInst);
}

void error(char *str)
{
    printf("%s",str);
    exit(1);
}

/* Acceptables :
 '0' to '9': integers
 '-': for negative numbers */
bool isDecimal(char* token)
{
    char c = token[0];
    return c>='0' && c<= '9' || c=='-';
}

//scan the assembly code and count the number of symbols
int findSymTabLen (FILE *inputFile)  // find lables
{
    int count = 0;
    int lineSize = 100;
    char line[50];
    while(fgets(line, lineSize, inputFile))       // fgets(name, size, input)
    {
        if((line[0] == ' ') || (line[0] == '\t'));  // so go to the next line
        else
        {
            count++; //counts the number of lines that start with a letter
        }
    }
    rewind(inputFile);  /*specified in the stdio. h library header file. The function moves the file position
    indicator to the beginning of the specified stream,
    while also clearing the error and EOF flags associated with that stream*/

    return count;
}

int fillSymTab (struct symbolTable *symT,FILE *inputFile)
{
    int lineNo = 0;
    int lineSize = 100;
    char line[50];
    int i=0;
    char *token;
    while(fgets(line, lineSize, inputFile))
    {
        if(( line[0] == ' ') || (line[0] == '\t'));
        /*the first non-empty expression in the line is extracted by the strtok() function
        and used as the symbol value in The symbol table is considered*/
        else
        {
            token = strtok(line,"\t, "); //strtok(char *str, const char *delim) --> tok str until delim
            strcpy(symT[i].symbol, token);
            symT[i++].value = lineNo;
        }
        lineNo++;
    }
    rewind(inputFile);

    return lineNo;
}

// hex to int
int hex2int(char *hex)
{
    int result = 0;

    while ((*hex) != '\0')
    {
        if(('0' <= (*hex)) && ((*hex) <= '9'))
        {
            result = result*16 + (*hex) - '0';
        }
        else if (('a' <= (*hex)) && ((*hex) <='f'))
        {
            result = result*16 + (*hex) - 'a' + 10;
        }
        else if (('A' <= (*hex)) && ((*hex) <='F'))
        {
            result = result*16 + (*hex) - 'A' + 10;
        }
        hex++;
    }

    return result;
}

//converts an int into hex.
void int2hex16(char *lower, int a) // changes for negative- numbers
{
    sprintf(lower, "%x", a); //sprintf(char *str, const char *format, ...) sends formatted output to a string pointed
    //convert the numerical value of `a` to its hex value, from `lower`

    if(a < 0x0) //input is negative, shows by shifting some hexadecimal digits resulting   < 0
    {
        lower[3] = lower[7];
        lower[2] = lower[6];
        lower[1] = lower[5];
        lower[0] = lower[4];
    }

    else if(a < 0x10)    // < 16
    {
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';

    }/*it has only one hex digit, then in order to determine the correct format,
        the houses corresponding to the digits zero and one in the `lower` array are manually set.*/

    else if(a < 0x100)   //  < 256
    {
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
    }
    else if(a < 0x1000)   // 4096
    {
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
    }

    lower[4] = '\0';
}
