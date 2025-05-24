#include "genericArray.h"
#include "analyser.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#define MAX_LINE_LEN 81
#define START_ADDRESS 100
#define MAX_LABEL_LEN 31


struct Symbol
{
    char name[MAX_LABEL_LEN];
    int address;
    int line;
    enum
    {
        symbolTypeEntry,
        symbolTypeExtern,
        symbolTypeCode,
        symbolTypeData,
        symbolTypeCodeEntry,
        symbolTypeDataEntry
    }symbolType;
    struct GenericArray *externalUse; /* a list of addresses that this external are refered to */
};


struct Prog
{
    struct GenericArray *code;
    struct GenericArray *data;
    struct GenericArray *symbolTable;
    unsigned int externUse:1; /* if 1, there is extern use in this program */
    unsigned int entriesExist; /* if 1, we have atleast 1 entry symbol in this program */
};


struct Symbol* symbolTableSearch(const struct GenericArray *symbolTable, const char *search);
void printError(const char *amFile, const int line, const char *convertor,...);
int firstPass(struct GenericArray *symbolTable, unsigned int *entriesExist, struct GenericArray *data, const char *amFileName, FILE *amFile);
int secondPass(struct Prog *program, const char *amFileName, FILE *amFile);
void print(struct Prog *program, const char *baseName);
