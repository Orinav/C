#ifndef LEXICAL_ANALYSIS_H
#define LEXICAL_ANALYSIS_H
#define MAX_LINE_LEN 81
#define SYNTAX_ERROR_LEN 161


enum instKey{MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP};
enum dirKey{DATA, STRING, ENTRY, EXTERN};


typedef struct LinePattern
{
    char *label;
    enum {DIRECTIVE, INSTRUCTION, SYNTAX_ERROR}
    DirectOrInstructOrError;
    union
    {
        char errorMessage[SYNTAX_ERROR_LEN];

        struct
        {
            enum instKey InstructionOpcode;
            struct
            {
                enum{IMMEDIATE = 1, DIRECT = 3, DIRECT_REGISTER = 5}
                OperandAddressing;
                union
                {
                    int number;
                    char *label;
                    int registerNumber;
                }
                OperandOptions;
            }OperandsQuantity[2];
        }Instruction;

        struct
        {
            enum dirKey directiveOptions;
            union
            {
                struct
                {
                    int data[MAX_LINE_LEN];
                    unsigned int dataCounter;
                }Data;
                char *label;
            } DirectiveInformation;
        }Directive;
    }Options;
} LinePattern;


LinePattern getLinePattern(char *line);

#endif
