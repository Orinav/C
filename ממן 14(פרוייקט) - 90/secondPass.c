#include "assembler.h"
#include "genericArray.h"
#include "analyser.h"
#include <stdio.h>
#include <string.h>


int secondPass(struct Prog *program, const char *amFileName, FILE *amFile)
{
    char line[MAX_LINE_LEN] = {0};
    LinePattern lp;
    struct Symbol *findSym;
    int lineCount = 1;
    int i;
    int  bits;
    int flag = 1;
    rewind(amFile);
    while(fgets(line, sizeof(line), amFile))
    {
        line[strcspn(line, "\r\n")] = 0;
        lp = getLinePattern(line);
        if(lp.DirectOrInstructOrError == INSTRUCTION)
        {
            bits = lp.Options.Instruction.InstructionOpcode << 5;
            bits |= lp.Options.Instruction.OperandsQuantity[1].OperandAddressing << 2;
            bits |= lp.Options.Instruction.OperandsQuantity[0].OperandAddressing << 9;
            addItem(program->code, &bits);
            if(lp.Options.Instruction.OperandsQuantity[1].OperandAddressing == DIRECT_REGISTER &&
               lp.Options.Instruction.OperandsQuantity[0].OperandAddressing == DIRECT_REGISTER)
            {
                bits = lp.Options.Instruction.OperandsQuantity[1].OperandOptions.registerNumber << 2;
                bits |= lp.Options.Instruction.OperandsQuantity[0].OperandOptions.registerNumber << 7;
                addItem(program->code, &bits);
            }
            else
            {
                for(i = 0; i < 2; i++)
                {
                    if(lp.Options.Instruction.OperandsQuantity[i].OperandAddressing == IMMEDIATE)
                        bits = lp.Options.Instruction.OperandsQuantity[i].OperandOptions.number << 2;
                    else if(lp.Options.Instruction.OperandsQuantity[i].OperandAddressing == DIRECT)
                    {
                        findSym = symbolTableSearch(program->symbolTable, lp.Options.Instruction.OperandsQuantity[i].OperandOptions.label);
                        if(findSym)
                        {
                            if(findSym->symbolType == symbolTypeExtern)
                            {
                                bits = getItemCounter(program->code) + START_ADDRESS;
                                if(findSym->externalUse == NULL)
                                    findSym->externalUse = createGenericArray(sizeof(int));
                                addItem(findSym->externalUse, &bits);
                                bits = 1;
                                program->externUse = 1;
                            }
                            else
                            {
                                bits = findSym->address;
                                bits <<= 2;
                                bits |= 2;
                            }
                        }
                        else
                        {
                            flag = 0;
                            printError(amFileName,lineCount, "undefined label:'%s'.", lp.Options.Instruction.OperandsQuantity[i].OperandOptions.label);
                        }
                    }
                    else if(lp.Options.Instruction.OperandsQuantity[i].OperandAddressing == DIRECT_REGISTER)
                    {
                        if(i == 0)
                            bits = lp.Options.Instruction.OperandsQuantity[0].OperandOptions.registerNumber << 7;
                        else
                            bits = lp.Options.Instruction.OperandsQuantity[1].OperandOptions.registerNumber << 4;
                    }
                    if(lp.Options.Instruction.OperandsQuantity[i].OperandAddressing >= IMMEDIATE)
                        addItem(program->code, &bits);
                 }
            }
        }
        lineCount++;
    }
    return flag;
}
