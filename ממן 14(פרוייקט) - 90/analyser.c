#include "analyser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#define JUMP_WHITESPACES(string) while(isspace(*string)) string++
#define ISSPACE_CHARS " \f\n\r\t\v"
#define MAX_NUM_VAL_INST 511  
#define MIN_NUM_VAL_INST -512
#define MAX_NUM_VAL_DATA 2047
#define MIN_NUM_VAL_DATA -2048
#define MAX_LABEL_LEN 31
#define MAX_REG_VAL 7
#define MIN_REG_VAL 0


struct asmKeyword
{
    enum {asmDir, asmInst}
    asmOption;
    const char *name;
    union
    {
        struct
        {
            enum instKey instKey;
            const char *srcOperandOptions; /*C - const, R - register , L - Label*/
            const char *destOperandOptions;
        }inst;
        struct
        {
            enum dirKey dirKey;
        }dir;
    }keywordUnion;
};


static struct asmKeyword asmKeywordTable[20] = 
{
    {asmInst,"mov",{{MOV,"CRL","RL"}}},
    {asmInst,"cmp",{{CMP,"CRL","CRL"}}},
    {asmInst,"add",{{ADD,"CRL","RL"}}},
    {asmInst,"sub",{{SUB,"CRL","RL"}}},
    {asmInst,"not",{{NOT,"","RL"}}},
    {asmInst,"clr",{{CLR,"","RL"}}},
    {asmInst,"lea",{{LEA,"R","RL"}}},
    {asmInst,"inc",{{INC,"","RL"}}},
    {asmInst,"dec",{{DEC,"","RL"}}},
    {asmInst,"jmp",{{JMP,"","RL"}}},
    {asmInst,"bne",{{BNE,"","RL"}}},
    {asmInst,"red",{{RED,"","RL"}}},
    {asmInst,"prn",{{PRN,"","CRL"}}},
    {asmInst,"jsr",{{JSR,"","RL"}}},
    {asmInst,"rts",{{RTS,"",""}}},
    {asmInst,"stop",{{STOP,"",""}}},

    {asmDir,".extern",{{EXTERN}}},
    {asmDir,".entry",{{ENTRY}}},
    {asmDir,".data",{{DATA}}},
    {asmDir,".string",{{STRING}}},
};


static int isValidLabel(const char *label)
{
    int c = 0;
    if (!isalpha(*label))
        return 0;
    c++;
    label++;
    while (isalnum(*label))
        label++;
    if (c > MAX_LABEL_LEN)
        return 0;
    if(*label != '\0')
        return 0;
    return 1;
}


static struct asmKeyword* keywordSearch(const char *keyword)
{
    int i;
    for(i = 0; i < 20; i++)
    {
        if(strcmp(asmKeywordTable[i].name, keyword) == 0)
            return &asmKeywordTable[i];
    }
    return NULL;
}


static int isValidNum(char *str, int *num, const int max, const int min, const int signCheck)
{
    char *ptr1;
    errno = 0;
    JUMP_WHITESPACES(str);
    if(*str == '\0')
        return 0;
    if(!signCheck)
    {
        if(*str == '+' || *str == '-')
            return 0;
    }
    *num = strtol(str,&ptr1,10);
    if(*ptr1 != '\0' && !isspace(*ptr1))
        return 0;
    JUMP_WHITESPACES(ptr1);
    if(*ptr1 != '\0')
        return 0;
    if(errno == ERANGE)
        return 0;
    if(*num > max || *num < min)
        return 0;
    return 1;
}


static char instStrParseOperand(char *operand, int *reg, int *number, char **label)
{
    char *ptr1, *ptr2;
    JUMP_WHITESPACES(operand);
    if(*operand == '@')
    {
        if(*(operand+1) != 'r')
            return 'U';
        if(!isValidNum(operand+2, reg, MAX_REG_VAL, MIN_REG_VAL, 0))
            return 'U';
        return 'R';
    }
    if(isValidNum(operand,number, MAX_NUM_VAL_INST, MIN_NUM_VAL_INST,1))
        return 'C';
    ptr1 = strpbrk(operand, ISSPACE_CHARS);
    if(ptr1) 
    {
        *ptr1 ='\0';
        ptr2 = ptr1;
        ptr1++;
        JUMP_WHITESPACES(ptr1);
        if(*ptr1 != '\0') 
        {
            *ptr2 = ' ';            
            return 'U';
        }
    }

    if(!isValidLabel(operand))
        return 'U';
    
    *label = operand;
    return 'L';
}


void static instOperandsStrparse(char *operandsContent, const struct asmKeyword *inst, LinePattern *lp)
{
    char *ptr1;
    int i;
    char got;
    int operandCount = (strlen(inst->keywordUnion.inst.destOperandOptions) >= 1) + (strlen(inst->keywordUnion.inst.srcOperandOptions) >= 1);
    JUMP_WHITESPACES(operandsContent);
    ptr1 = strchr(operandsContent, ',');
    if(operandCount == 2)
    {
        if(ptr1 == NULL)
        {
            lp->DirectOrInstructOrError = SYNTAX_ERROR;
            sprintf(lp->Options.errorMessage,"instruction:'%s' expected separator ','.", inst->name);
            return;
        }
    }
    else if(ptr1 != NULL)
    {
        lp->DirectOrInstructOrError = SYNTAX_ERROR;
        sprintf(lp->Options.errorMessage,"instruction:'%s' got separator: ','", inst->name);
        return;
    }

    switch (operandCount)
    {
        case 0:
            if(*operandsContent != '\0')
            {
                lp->DirectOrInstructOrError = SYNTAX_ERROR;
                sprintf(lp->Options.errorMessage,"instruction:'%s' got operand but expected none", inst->name);
                return;
            }
        break;
        case 1:
            got = instStrParseOperand(operandsContent,
            &lp->Options.Instruction.OperandsQuantity[1].OperandOptions.registerNumber,
                &lp->Options.Instruction.OperandsQuantity[1].OperandOptions.number,
                &lp->Options.Instruction.OperandsQuantity[1].OperandOptions.label);
            if(got == 'U')
            {
                lp->DirectOrInstructOrError = SYNTAX_ERROR;
                sprintf(lp->Options.errorMessage,"instruction:'%s' got unknown operand:'%s'.", inst->name, operandsContent);
                return;   
            }
            if(strchr(inst->keywordUnion.inst.destOperandOptions, got) == NULL)
            {
                lp->DirectOrInstructOrError = SYNTAX_ERROR;
                sprintf(lp->Options.errorMessage,"instruction:'%s' got destination operand:'%s' but this instruction does not support it", inst->name, operandsContent );
                return;
            }
            lp->Options.Instruction.OperandsQuantity[1].OperandAddressing = got == 'C' ? IMMEDIATE : got == 'R' ? DIRECT_REGISTER : DIRECT;
        break;
        case 2:
            *ptr1 ='\0';
            for(i = 0; i < 2; i++)
            {
                got = instStrParseOperand(i == 0 ? operandsContent : ptr1+1,
              &lp->Options.Instruction.OperandsQuantity[i].OperandOptions.registerNumber,
                  &lp->Options.Instruction.OperandsQuantity[i].OperandOptions.number,
                  &lp->Options.Instruction.OperandsQuantity[i].OperandOptions.label);
                if(got == 'U')
                {
                    lp->DirectOrInstructOrError = SYNTAX_ERROR;
                    sprintf(lp->Options.errorMessage,"instruction:'%s' got unknown operand:'%s'.", inst->name, i == 0 ? operandsContent : ptr1+1);
                    return;
                }
                if(strchr(i==0 ?  inst->keywordUnion.inst.srcOperandOptions : inst->keywordUnion.inst.destOperandOptions, got) == NULL)
                {
                    lp->DirectOrInstructOrError = SYNTAX_ERROR;
                    sprintf(lp->Options.errorMessage,"instruction:'%s' got %s operand:'%s' but this instruction does not support it", inst->name, i== 0 ? "source" : "destination", i == 0 ? operandsContent : ptr1+1);
                    return;
                }
                lp->Options.Instruction.OperandsQuantity[i].OperandAddressing = got == 'C' ? IMMEDIATE : got == 'R' ? DIRECT_REGISTER : DIRECT;
            }
        break;
    }
}


 static void dirOperandStrParse(char *operandContent, const enum dirKey dirKey, LinePattern *lp)
 {
    char *ptr1, *ptr2;
    JUMP_WHITESPACES(operandContent);
    switch (dirKey)
    {
        case EXTERN: case ENTRY:
        ptr1 = strpbrk(operandContent, ISSPACE_CHARS);
        if(ptr1)
            *ptr1 = '\0';
        if(!isValidLabel(operandContent))
        {
            lp->DirectOrInstructOrError = SYNTAX_ERROR;
            sprintf(lp->Options.errorMessage, "invalid label name:'%s'.", operandContent);
            return;
        }
        lp->Options.Directive.DirectiveInformation.label = operandContent;
        break;
        case STRING:
        ptr1 = operandContent;
        if(*ptr1 != '"')
        {
            lp->DirectOrInstructOrError = SYNTAX_ERROR;
            sprintf(lp->Options.errorMessage, "no opening: '\"' in .string.");
            return;
        }
        ptr2 = strrchr(ptr1+1,'"');
        if(ptr2 == NULL )
        {
            lp->DirectOrInstructOrError = SYNTAX_ERROR;
            sprintf(lp->Options.errorMessage, "no closing: '\"' in .string.");
            return;
        }
        *ptr2 = '\0';
        ptr2++;
        JUMP_WHITESPACES(ptr2);
        if(*ptr2 != '\0')
        {
            lp->DirectOrInstructOrError = SYNTAX_ERROR;
            sprintf(lp->Options.errorMessage,"extra text after .string :'%s'.", ptr2);
            return;
        }
        ptr1++;
        while(*ptr1)
        {
            lp->Options.Directive.DirectiveInformation.Data.data[lp->Options.Directive.DirectiveInformation.Data.dataCounter++] = *ptr1;
            ptr1++;
        }
        lp->Options.Directive.DirectiveInformation.Data.data[lp->Options.Directive.DirectiveInformation.Data.dataCounter++] = *ptr1;
        break;
        case DATA:
        if(*operandContent == '\0')
        {
            lp->DirectOrInstructOrError = SYNTAX_ERROR;
            sprintf(lp->Options.errorMessage,".data directive expects atleast 1 number. ");
            return;
        }
        do
        {
            ptr1 = strchr(operandContent,',');
            if(ptr1)
                *ptr1 = '\0';
            if(!isValidNum(operandContent, 
                        &lp->Options.Directive.DirectiveInformation.Data.data[lp->Options.Directive.DirectiveInformation.Data.dataCounter], 
                        MAX_NUM_VAL_DATA, MIN_NUM_VAL_DATA, 1))
                        {
                            lp->DirectOrInstructOrError = SYNTAX_ERROR;
                            sprintf(lp->Options.errorMessage,"bad number:'%s'.", operandContent);
                            return;
                        }
            lp->Options.Directive.DirectiveInformation.Data.dataCounter++;
            operandContent = ptr1 +1;
        } while(ptr1);
        break;
    }
}

LinePattern getLinePattern(char *line)
{
    struct LinePattern lp = {0};
    struct asmKeyword * keyWordPtr;
    char *ptr1;
    JUMP_WHITESPACES(line);
    ptr1 = strchr(line,':');

    if(ptr1)
    {
        *ptr1 = '\0';
        lp.label = line;
        if(!isValidLabel(line))
        {
            sprintf(lp.Options.errorMessage, "bad label name:'%s'.", line);
            lp.DirectOrInstructOrError = SYNTAX_ERROR;
            return lp;
        }
        line = ptr1 + 1;
    }
    JUMP_WHITESPACES((line));
    if(*line == '\0')
    {
        lp.DirectOrInstructOrError = SYNTAX_ERROR;
        sprintf(lp.Options.errorMessage,"line contains only a label.");
        return lp;
    }
    ptr1 = strpbrk(line, ISSPACE_CHARS);
    if(ptr1)
        *ptr1 = '\0';
    keyWordPtr = keywordSearch(line);
    if(!keyWordPtr)
    {
        lp.DirectOrInstructOrError = SYNTAX_ERROR;
        sprintf(lp.Options.errorMessage,"undefined keyword:'%s'.", line);
        return lp;
    }

    if(keyWordPtr->asmOption == asmInst)
    {
        lp.DirectOrInstructOrError = INSTRUCTION;
        lp.Options.Instruction.InstructionOpcode = keyWordPtr->keywordUnion.inst.instKey;
        if(ptr1)
            instOperandsStrparse(ptr1+1, keyWordPtr, &lp);
        else if(keyWordPtr->keywordUnion.inst.srcOperandOptions[0] != '\0')
        {
            lp.DirectOrInstructOrError = SYNTAX_ERROR;
            sprintf(lp.Options.errorMessage, "instruction:'%s' got no operands.", keyWordPtr->name);
            return lp;
        }
    }
    else
    {
        lp.DirectOrInstructOrError = DIRECTIVE;
        lp.Options.Directive.directiveOptions = keyWordPtr->keywordUnion.dir.dirKey;
        if(ptr1)
            dirOperandStrParse(ptr1+1, keyWordPtr->keywordUnion.dir.dirKey, &lp);
        else
        {
            lp.DirectOrInstructOrError = SYNTAX_ERROR;
            sprintf(lp.Options.errorMessage, "directive:'%s' got no operands.", keyWordPtr->name);
            return lp;
        }
    }
    return lp;
}
