#include "macro.h"
#include "genericArray.h"
#include "commonFunctions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_LINE_LEN 81 
#define ISSPACE_CHARS " \f\n\r\t\v"


struct Macro
{
    char name[MAX_LINE_LEN];
    struct GenericArray *macroContent;
};


typedef enum Status
{
    MACRO_DEFINITION, END_MACRO, MACRO_ASSIGN, OTHER, EMPTY_LINE, MACRO_ERROR
} Status;


static int nameOfInstruction(const char *macroName)
{
    if (strcmp(macroName, "mov") == 0
    || strcmp(macroName, "cmp") == 0
    || strcmp(macroName, "add") == 0
    || strcmp(macroName, "sub") == 0
    || strcmp(macroName, "not") == 0
    || strcmp(macroName, "clr") == 0
    || strcmp(macroName, "lea") == 0
    || strcmp(macroName, "inc") == 0
    || strcmp(macroName, "dec") == 0
    || strcmp(macroName, "jmp") == 0
    || strcmp(macroName, "bne") == 0
    || strcmp(macroName, "red") == 0
    || strcmp(macroName, "prn") == 0
    || strcmp(macroName, "jsr") == 0
    || strcmp(macroName, "rts") == 0
    || strcmp(macroName, "stop") == 0)
        return 1;
    return 0;
}


static struct Macro* findMacro(const struct GenericArray *macroArray, const char *name)
{
    int i,s;
    struct Macro *tempMacro;
    GenericArrayLoop(i,tempMacro,s,macroArray)
    {
        if(strcmp(tempMacro[i].name, name) == 0)
            return &tempMacro[i];
    }
    return NULL;
}


static Status getLineStatus(char *line, struct Macro **macro, struct GenericArray *macroArray)
{
    char *cursor;
    char *macroName;
    struct Macro *getMacro;
    struct Macro localMacro = {0};
    while (isspace(*line))
        line++;
    if(*line == '\0')
        return EMPTY_LINE;
    cursor = strstr(line, "endmcro");
    if(cursor)
    {
        if(cursor != line)
            return MACRO_ERROR;
        cursor = line + 7;
        while(isspace(*cursor))
            cursor++;
        if(*cursor != '\0')
            return MACRO_ERROR;
        *macro = NULL;
        return END_MACRO;
    }
    cursor = strstr(line, "mcro");
    if(cursor)
    {
        if(cursor != line)
            return MACRO_ERROR;
        cursor = strpbrk(line, ISSPACE_CHARS);
        if(cursor == NULL)
            return MACRO_ERROR;
        while(isspace(*cursor))
            cursor++;
        if(*cursor == '\0')
            return MACRO_ERROR;
        macroName = cursor;
        cursor = strpbrk(cursor, ISSPACE_CHARS);
        if(cursor)
            *cursor = '\0';
        cursor++;
        while(isspace(*cursor))
            cursor++;
        if(*cursor != '\0')
            return MACRO_ERROR;
        getMacro = findMacro(macroArray, macroName);
        if(getMacro)
            return MACRO_ERROR;
        strcpy(localMacro.name, macroName);
        if (nameOfInstruction(localMacro.name))
            return MACRO_ERROR; /* macro name is a name of an instruction */
        localMacro.macroContent = createGenericArray(sizeof(char*));
        *macro = addItem(macroArray, &localMacro);
        return MACRO_DEFINITION;
    }
    cursor = strpbrk(line, ISSPACE_CHARS);
    if(cursor)
        *cursor = '\0';
    getMacro = findMacro(macroArray, line);
    if(getMacro)
    {
        if(cursor)
        {
            cursor++;
            while(isspace(*cursor))
                cursor++;
            if(*cursor != '\0')
                return MACRO_ERROR;
        }
        *macro = getMacro;
        return MACRO_ASSIGN;
    }
    return OTHER;
}


const char* preProcessor(const char *fileName)
{
   char line[MAX_LINE_LEN] = {0};
   FILE *asFile = NULL;
   FILE *amFile = NULL;
   int lineCounter = 1;
   int i, j, s1, s2;
   int errorMacroFlag = 0;
   struct GenericArray *macroArray;
   char *lineDuplication;
   char **tempLine;
   char *comment;
   Status lineStatus;
   struct Macro *macro = NULL; /* if NULL then we are not in macro definition block or macro definition line */
   char *asFileName = myStrcat(fileName, ".as");
   char *amFileName = myStrcat(fileName, ".am");

   asFile = fopen(asFileName, "r");
   amFile = fopen(amFileName, "w");
   
   if(asFile == NULL || amFile == NULL) 
   { 
    free(amFileName);
    free(asFileName);
    if(amFile)
        fclose(amFile);
    if(asFile)
        fclose(asFile);
    return NULL;
   }
   
   macroArray = createGenericArray(sizeof(struct Macro));
   while (fgets(line, sizeof(line), asFile) != NULL)
   {
       comment = strchr(line,';');
       if(comment)
           *comment = '\0';
       lineDuplication = strcpy(malloc(strlen(line) + 1), line);
       switch(lineStatus = getLineStatus(line, &macro, macroArray))
       {
            case EMPTY_LINE:
            break;
            case MACRO_ERROR:
                lineDuplication[strcspn(lineDuplication, "\r\n")] = 0;
                printf("error in macro line :'%s'.\n", lineDuplication);
                errorMacroFlag = 1;
            break;
           case (MACRO_DEFINITION):
               break;
           case (END_MACRO):
               break;
           case (MACRO_ASSIGN):
               GenericArrayLoop(i,tempLine,s1,macro->macroContent)
                    fputs(tempLine[i], amFile);
               macro = NULL;
               break; 
           case (OTHER):
               if (macro == NULL)
               {
                   fputs(lineDuplication, amFile);
                   free(lineDuplication);
               }
               else
                   addItem(macro->macroContent, &lineDuplication);
               break;
       }
       if(lineStatus != OTHER)
            free(lineDuplication);
       lineCounter++;
   }
    GenericArrayLoop(i,macro,s1,macroArray)
    {
        GenericArrayLoop(j,tempLine,s2,macro[i].macroContent)
            free(tempLine[j]);
        freeGenericArray(&macro[i].macroContent);
   }
   freeGenericArray(&macroArray);
   fclose(asFile);
   fclose(amFile);
   free(asFileName);
   if(errorMacroFlag) 
   {
        remove(amFileName);
        free(amFileName);
        amFileName = NULL;
   }
   return amFileName;
}

