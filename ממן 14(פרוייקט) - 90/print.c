#include "assembler.h"
#include "genericArray.h"
#include "commonFunctions.h"
#include <stdio.h>
#include <stdlib.h>


static void printEntry(const struct GenericArray *symbolTable, const char *entryFileName)
{
    int i,s;
    FILE *entryFile;
    const struct Symbol *tempSymbol;
    entryFile = fopen(entryFileName, "w");
    if(entryFile == NULL)
        return;
    GenericArrayLoop(i,tempSymbol,s,symbolTable) 
    {
        if(tempSymbol[i].symbolType >= symbolTypeCodeEntry)
            fprintf(entryFile, "%s\t%d\n", tempSymbol[i].name, tempSymbol[i].address);
    }
    fclose(entryFile);
}


static void printExtern(const struct GenericArray *symbolTable, const char *externFileName)
{
    int i,s1,j,s2;
    FILE *extFile;
    const struct Symbol *tempSymbol;
    int *address;
    extFile = fopen(externFileName, "w");
    if(extFile == NULL)
        return;
    GenericArrayLoop(i,tempSymbol,s1,symbolTable)
    {
        if(tempSymbol[i].symbolType == symbolTypeExtern && tempSymbol[i].externalUse != NULL)
        {
            GenericArrayLoop(j, address, s2, tempSymbol[i].externalUse)
                fprintf(extFile, "%s\t%d\n", tempSymbol[i].name, address[j]);
        }
    }
    fclose(extFile);   
}


static void printOb(const struct GenericArray *code, const struct GenericArray *data , const char *objectFileName)
{
    char base64Table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
    int i,s;
    const int mask = 0x3F;
    FILE * obFile;
    int *bits;
    obFile = fopen(objectFileName,"w");
    if(obFile == NULL)
        return;
    GenericArrayLoop(i,bits,s,code) 
        fprintf(obFile,"%c%c\n",base64Table[(bits[i]>> 6 ) & mask],base64Table[(bits[i]) & mask]);
    GenericArrayLoop(i,bits,s,data)
        fprintf(obFile,"%c%c\n",base64Table[(bits[i]>> 6 ) & mask],base64Table[(bits[i]) & mask]);
    fclose(obFile);
}


void print(struct Prog *program, const char *baseName)
{
    const char *obFileName, *extFileName, *entFileName;
    if(program->externUse) 
    {
        extFileName = myStrcat(baseName, ".ext");
        printExtern(program->symbolTable, extFileName);
        free((void*)extFileName); 
    }
    
    if(program->entriesExist) 
    {
        entFileName = myStrcat(baseName, ".ent");
        printEntry(program->symbolTable, entFileName);
        free((void*)entFileName);
    }
    obFileName = myStrcat(baseName, ".ob");
    printOb(program->code, program->data, obFileName); 
    free((void*)obFileName);
}
