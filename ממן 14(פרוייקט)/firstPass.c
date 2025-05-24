#include "assembler.h"
#include "genericArray.h"
#include <string.h>


int firstPass(struct GenericArray *symbolTable,unsigned int *entriesExist, struct GenericArray *data, const char *amFileName, FILE *amFile)
{
    char line[MAX_LINE_LEN] = {0};
    LinePattern lp;
    struct Symbol tempSym = {0};
    struct Symbol *findSym;
    int lineCount = 1;
    int IC = START_ADDRESS;
    int DC = 0;
    int flag = 1;
    int i,s;
    while(fgets(line, sizeof(line), amFile) != NULL) 
    {
        line[strcspn(line, "\r\n")] = 0;
        lp = getLinePattern(line);
        if(lp.DirectOrInstructOrError != SYNTAX_ERROR &&  lp.label && 
            (lp.DirectOrInstructOrError == INSTRUCTION || lp.Options.Directive.directiveOptions <= STRING )) 
            {
            findSym =symbolTableSearch(symbolTable, lp.label);
            if(findSym)
            {
                if(findSym->symbolType == symbolTypeEntry) 
                {
                    if(lp.DirectOrInstructOrError == DIRECTIVE) 
                    {
                        findSym->symbolType = symbolTypeDataEntry; 
                        findSym->address = DC;
                    }
                    else
                        findSym->symbolType = symbolTypeCodeEntry;
                }
                else
                {
                    flag = 0;
                    printError(amFileName, lineCount, "label redefinition:'%s' was already defined in line:%d", findSym->name, findSym->line);
                }
            }
            else
            {
                strcpy(tempSym.name, lp.label); 
                tempSym.line = lineCount;
                tempSym.address = lp.DirectOrInstructOrError == DIRECTIVE ? DC : IC;
                tempSym.symbolType = lp.DirectOrInstructOrError == DIRECTIVE ? symbolTypeData : symbolTypeCode;
                addItem(symbolTable, &tempSym);  
            }
        }
        switch (lp.DirectOrInstructOrError)
        {
        case INSTRUCTION:
            if(lp.Options.Instruction.OperandsQuantity[0].OperandAddressing == DIRECT_REGISTER &&
                lp.Options.Instruction.OperandsQuantity[1].OperandAddressing == DIRECT_REGISTER) 
                    IC += 2;
                else
                {
                    IC += 1 + (lp.Options.Instruction.OperandsQuantity[0].OperandAddressing >= IMMEDIATE) +
                      (lp.Options.Instruction.OperandsQuantity[1].OperandAddressing >= IMMEDIATE);
                }
            break;
        case DIRECTIVE:
            if(lp.Options.Directive.directiveOptions <= STRING) 
            {
                DC += lp.Options.Directive.DirectiveInformation.Data.dataCounter;
                for(i = 0; i < lp.Options.Directive.DirectiveInformation.Data.dataCounter; i++)
                    addItem(data, &lp.Options.Directive.DirectiveInformation.Data.data[i]);
                break;
            }
            else /* entry or extern */
            { 
                findSym = symbolTableSearch(symbolTable, lp.Options.Directive.DirectiveInformation.label);
                if(findSym != NULL && lp.Options.Directive.directiveOptions == EXTERN)
                {
                        flag = 0;
                        printError(amFileName, lineCount, "label redefinition:'%s' was already defined in line:%d", findSym->name, findSym->line);
                }
                else if(findSym)
                {
                    if(findSym->symbolType == symbolTypeData)
                        findSym->symbolType = symbolTypeDataEntry;
                    else if(findSym->symbolType == symbolTypeCode)
                        findSym->symbolType = symbolTypeCodeEntry;
                    else
                    {
                        flag = 0;
                        printError(amFileName, lineCount, "label redefinition:'%s' was already defined in line:%d", findSym->name, findSym->line);
                    }
                }
                else
                {
                    strcpy(tempSym.name, lp.Options.Directive.DirectiveInformation.label);
                    tempSym.line = lineCount;
                    tempSym.address = 0;
                    tempSym.symbolType = lp.Options.Directive.directiveOptions == EXTERN ? symbolTypeExtern : symbolTypeEntry;
                    addItem(symbolTable, &tempSym);
                }
            }
            break;
        case SYNTAX_ERROR:
            flag = 0;
            printError(amFileName, lineCount, "syntax:'%s'.", lp.Options.errorMessage);
            break;
        default:
            break;
        }
        lineCount++;
    }


    GenericArrayLoop(i,findSym,s,symbolTable)
    {
        switch (findSym[i].symbolType)
        {
        case symbolTypeEntry:
                printError(amFileName, lineCount, "label:'%s' was declared entry in line:'%d' but was never defined in this file", findSym[i].name, findSym[i].line);
                flag = 0; 
            break;
        case symbolTypeDataEntry: case symbolTypeCodeEntry:
            *entriesExist = 1;
        case symbolTypeData:
            if(findSym[i].symbolType != symbolTypeCodeEntry)
                findSym[i].address += IC;
        break;
        default:
            break;
        }
    }
    return flag;
}
