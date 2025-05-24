#include "assembler.h"
#include "macro.h"
#include <string.h>
#include <stdlib.h>


static struct Prog createProgStruct()
{
    struct Prog newProg;
    newProg.code = createGenericArray(sizeof(int));
    newProg.data = createGenericArray(sizeof(int));
    newProg.symbolTable = createGenericArray(sizeof(struct Symbol));
    return newProg;
}


static void freeStructProg(struct Prog *prog)
{
    int i,s;
    struct Symbol *tempSymbol;
    freeGenericArray(&prog->code);
    freeGenericArray(&prog->data);
    if(prog->externUse)
    {
        GenericArrayLoop(i,tempSymbol,s,prog->symbolTable) 
        {
            if(tempSymbol[i].externalUse != NULL)
                freeGenericArray(&tempSymbol[i].externalUse);
        }
    }
    freeGenericArray(&prog->symbolTable);
}


struct Symbol* symbolTableSearch(const struct GenericArray *symbolTable, const char *search)
{
    int i,s;
    struct Symbol *tempSymbol;
    GenericArrayLoop(i,tempSymbol,s,symbolTable)
    {
        if(strcmp(search, tempSymbol[i].name) == 0)
            return &tempSymbol[i];
    }
    return NULL;
}


void printError(const char *amFile, const int line, const char *convertor,...) 
{
    va_list args;
    printf("%s:%d: error:", amFile, line);
    va_start(args, convertor);
    vprintf(convertor, args);
    va_end(args);
    printf("\n");
}


int main(int argc, char **argv) 
{
    int i;
    const char *amFileName;
    struct Prog program;
    FILE *amFile;
    for(i = 1; i < argc; i++) 
    {
        amFileName = preProcessor(argv[i]);
        amFile = fopen(amFileName,"r");
        if(amFileName && amFile) 
        {
            program = createProgStruct();
            if(firstPass(program.symbolTable, &program.entriesExist, program.data, amFileName, amFile)) 
            {
                if(secondPass(&program, amFileName, amFile))
                    print(&program,argv[i]);
            }
            freeStructProg(&program);
            fclose(amFile);
        }
        if(amFileName)
            free((void*)amFileName);
    }
    return 0;
}
