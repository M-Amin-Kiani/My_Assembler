//Mohammad Amin Kiani 4003613052

#include "isa.h" //Functions and structures required by the program

int main(int argc, char **argv)
{
    FILE *readptr, *writeptr;
    char *hexNumber = (char *)malloc(8);
    struct InsMem *currInst = (struct InsMem *)malloc(sizeof(struct InsMem));
    struct RegFile *Regs = (struct RegFile *)malloc(sizeof(struct RegFile));
    struct Adder *adder = (struct Adder *)malloc(sizeof(struct Adder));
    struct ALU *alu = (struct ALU *)malloc(sizeof(struct ALU));
    struct DataMem *dataMem = (struct DataMem *)malloc(sizeof(struct DataMem));

    adder->PC = 0;
    dataMem->memUsed = 0;
    alu->isHalted = false;
    size_t i = 0, j = 0, InstCount = 0, count = 0, inputLines = 0;

    char HexChar[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    if (argc < 3)
    {
        printf("***** Please run this program as follows: \n");
        printf("***** %s assprog.as machprog.m\n", argv[0]);
        printf("***** where assprog.as is your assembly program\n");
        printf("***** and machprog.m will be your machine code.\n");
        exit(1);
    }
    if ((readptr = fopen(argv[1], "r")) == NULL)
    {
        printf("%s cannot be opened\n", argv[1]);
        exit(1);
    }
    if ((writeptr = fopen(argv[2], "w+")) == NULL)
    {
        printf("%s cannot be opened\n", argv[2]);
        exit(1);
    }
    for (i = 0; i < 16; i++)
    {
        fprintf(writeptr, "R%i\t", i); // reg number
    }

    fprintf(writeptr, "Reg Used(%%)\t");
    fprintf(writeptr, "Instr Count\t");
    fprintf(writeptr, "Mem Used\t");
    fprintf(writeptr, "\n");
    fprintf(writeptr, "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    //---------------------------------------------------------------------------
    for (i = 0; i < 16; i++)
    {
        Regs->Registers[i] = 0;
        Regs->isUsed[i] = false;
        fprintf(writeptr, "%d\t", Regs->Registers[i]);
    }

    // first values...
    fprintf(writeptr, "%d\t\t", count);
    fprintf(writeptr, "%d\t\t", InstCount);
    fprintf(writeptr, "%d", dataMem->memUsed);
    fprintf(writeptr, "\n");

    // count the lines of input file
    char c;

    for (c = getc(readptr); c != EOF; c = getc(readptr))
        if (c == '\n')
            count += 1;

    currInst->inputList = ReadFile(readptr, count);
    inputLines = count;
    i = 0;
    int OpCode;
    while (1)
    {
        i = adder->PC;
        int2hex16(hexNumber, currInst->inputList[i]);
        for (j = 0; j < 15; j++)
        {
            if (hexNumber[0] == HexChar[j])
            {
                OpCode = j;
                break;
            }
        }

        // Zero Extend
        if (OpCode == 14 || OpCode == 7)
        {
            currInst->imm[0] = '0';
            currInst->imm[1] = '0';
            currInst->imm[2] = '0';
            currInst->imm[3] = '0';

            for (j = 4; j < 8; j++)
                currInst->imm[j] = hexNumber[j - 1];
        }

        // Sign Extend
        else
        {
            if((hexNumber[3] - '0') > 7)
            {
                currInst->imm[0] = 'F';
                currInst->imm[1] = 'F';
                currInst->imm[2] = 'F';
                currInst->imm[3] = 'F';
            }
            else
            {
                currInst->imm[0] = '0';
                currInst->imm[1] = '0';
                currInst->imm[2] = '0';
                currInst->imm[3] = '0';
            }
            for (j = 4; j < 8; j++)
                currInst->imm[j] = hexNumber[j - 1];
        }
        
        

        if (OpCode >= 0 && OpCode < 5)
        {

            // operation
            switch (OpCode)
            {
            case 0: //add
                Regs->Registers[hexNumber[3] - '0'] = Regs->Registers[hexNumber[1] - '0'] + Regs->Registers[hexNumber[2] - '0'];
                break;
            case 1: // sub
                Regs->Registers[hexNumber[3] - '0'] = Regs->Registers[hexNumber[1] - '0'] - Regs->Registers[hexNumber[2] - '0'];
                break;
            case 2:  // slt
                if (Regs->Registers[hexNumber[1] - '0'] < Regs->Registers[hexNumber[2] - '0'])
                {
                    Regs->Registers[hexNumber[3] - '0'] = 1;
                }
                else
                {
                    Regs->Registers[hexNumber[3] - '0'] = 0;
                }
                break;
            case 3:  //or
                Regs->Registers[hexNumber[3] - '0'] = Regs->Registers[hexNumber[1] - '0'] | Regs->Registers[hexNumber[2] - '0'];
                break;

            case 4: // nand
                Regs->Registers[hexNumber[3] - '0'] = ~(Regs->Registers[hexNumber[1] - '0'] & Regs->Registers[hexNumber[2] - '0']);
                break;
            }
            Regs->isUsed[hexNumber[3] - '0'] = true;
        }
        else if (OpCode >= 5 && OpCode < 13)
        {
            switch (OpCode)
            {
            case 5: //addi
                Regs->Registers[hexNumber[2] - '0'] = Regs->Registers[hexNumber[1] - '0'] + hex2int(currInst->imm);
                break;
            case 6:  //slti
                if (Regs->Registers[hexNumber[1] - '0'] < hex2int(currInst->imm))
                {
                    Regs->Registers[hexNumber[2] - '0'] = 1;
                }
                else
                {
                    Regs->Registers[hexNumber[2] - '0'] = 0;
                }
                break;
            case 7: // ori
                Regs->Registers[hexNumber[2] - '0'] = Regs->Registers[hexNumber[1] - '0'] | hex2int(currInst->imm);
                break;
            case 8: // lui
                Regs->Registers[hexNumber[2] - '0'] = hex2int(currInst->imm) << 16;
                break;
            case 9:  // lw
                Regs->Registers[hexNumber[2] - '0'] = currInst->inputList[hex2int(currInst->imm) + Regs->Registers[hexNumber[1] - '0']];
                dataMem->memUsed++;
                break;
            case 10: // sw
                currInst->inputList[hex2int(currInst->imm) + Regs->Registers[hexNumber[1] - '0']] = Regs->Registers[hexNumber[2] - '0'];
                dataMem->memUsed++;
                break;
            case 11:  // beq
                if (Regs->Registers[hexNumber[1] - '0'] == Regs->Registers[hexNumber[2] - '0'])
                {
                    adder->PC = adder->PC + 1 + hex2int(currInst->imm);
                }
                else
                {
                    adder->PC = adder->PC + 1;
                }
                break;
            case 12:  // jalr
                Regs->Registers[hexNumber[2] - '0'] = adder->PC + 1;
                adder->PC = Regs->Registers[hexNumber[1] - '0'];
                break;
            }
            if(OpCode != 10 && OpCode != 11) // need re file ( not just mem! )
            {
                Regs->isUsed[hexNumber[2] - '0'] = true;
            }
        }
        else
        {
            switch (OpCode)
            {
            case 13: // j
                adder->PC = hex2int(currInst->imm);
                break;
            case 14:  // halt
                alu->isHalted = true;
                break;
            }
        }
        Regs->Registers[0] = 0;
        Regs->isUsed[0] = false;

        count = 0; // re.use this
        for ( i = 0; i < 16; i++)
        {
            if(Regs->isUsed[i])
            {
                count++;
            }
        }
        
        // print to the file
        for (i = 0; i < 16; i++)
        {
            fprintf(writeptr, "%d\t", Regs->Registers[i]);
        }
        fprintf(writeptr, "%3.2f\t\t", count*100/16.0); // pers%   ( 2*100 / 16 ) = 12.50
        fprintf(writeptr, "%d\t\t", ++InstCount);   // 0 , 1 , 2 , ...


        if (!(OpCode == 12 || OpCode == 11 || OpCode == 13 || OpCode == 14))
        {
            adder->PC = adder->PC + 1;
        }
        if (alu->isHalted)
        {
            int dotFills = inputLines - adder->PC - 1 + dataMem->memUsed;
            fprintf(writeptr, "%d", dotFills);
            break;
        }

        fprintf(writeptr, "%d", dataMem->memUsed);
        fprintf(writeptr, "\n");
    }


    //freeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
    fclose(readptr);
    fclose(writeptr);
    free(currInst);
    free(Regs);
    free(alu);
    free(adder);
    free(hexNumber);


    return 0;
}

//-------------------------------------------------------------------------------------------

int *ReadFile(FILE *inputFile, int count)
{
    int i = 0;
    int *List = (int *)malloc(count * sizeof(int));
    count = 0;

    fseek(inputFile, 0, SEEK_SET);
    while (!feof(inputFile))
    {
        fscanf(inputFile, "%i", &i);
        List[count++] = i;
    }
    return List;
}

int hex2int(char *hex)
{
    int result = 0;
    while ((*hex) != '\0')
    {
        if (('0' <= (*hex)) && ((*hex) <= '9'))
            result = result * 16 + (*hex) - '0';
        else if (('a' <= (*hex)) && ((*hex) <= 'f'))
            result = result * 16 + (*hex) - 'a' + 10;
        else if (('A' <= (*hex)) && ((*hex) <= 'F'))
            result = result * 16 + (*hex) - 'A' + 10;
        hex++;
    }

    return result;
}

void int2hex16(char *lower, int a) // but some changes for negative numbers
{
    sprintf(lower, "%X", a);
    if (a < 0x0)
    {
        lower[3] = lower[7];
        lower[2] = lower[6];
        lower[1] = lower[5];
        lower[0] = lower[4];
        lower[4] = '\0';
    }
    else if (a < 0x10)
    {
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';
        lower[4] = '\0';
    }
    else if (a < 0x100)
    {
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
        lower[4] = '\0';
    }
    else if (a < 0x1000)
    {
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
        lower[4] = '\0';
    }

    if(lower[6] == '\0')
    {
        int i;
        for ( i = 5; i >= 0; i--)
        {
            lower[i + 1] = lower[i];
        }
        lower[0] = '0';
    }
}
