#ifndef INTER_H
#define INTER_H

#include "semantic.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct InterCodes_* InterCodes;
struct Operand_ {
    enum { TEMP, VARIABLE, CONSTANT, ADDRESS, OP_LABEL, OP_FUNC, OP_RELOP } kind;
    union {
        char *name;
        int var_no;
        int value;
    } u;
};

struct InterCode_
{
    enum { LABEL, FUNC, ASSIGN, ADD, SUB, MUL, CODE_DIV, GET_AD, READ_AD, WRITE_AD
    , GOTO, CODE_IF, CODE_RETURN, DEC, ARG, CALL, PARAM, READ, WRITE } kind;
    union {
        struct { Operand val; } sinop;
        struct { Operand right, left; } binop;
        struct { Operand result, op1, op2; } triop;
        struct { Operand x, relop, y, z; } conjump;
        struct { Operand x; int size; } dec;
    } u;
};

struct InterCodes_ { InterCode code; struct InterCodes_ *prev, *next; };

Operand newOperand(int kind, ...);
void deleteOperand(Operand op);
void fprintOperand(char* t, Operand op);

InterCode newInterCode(int kind, ...);
void deleteInterCode(InterCode ic);
void fprintInterCode(InterCode ic, FILE* f);

InterCodes newInterCodes();
void addInterCodes(InterCodes ics, InterCode code);
void deleteInterCodes(InterCodes ics);
void fprintInterCodes(InterCodes ics, FILE* f);

void transProgram(Tree root); // the entry of semantic analyze
void transExtDefList(Tree root);
void transExtDef(Tree root);

void transVarDec(Tree root); // insert an item(varity defination)

void transFunDec(Tree root); // insert an item(function defination)
void transVarList(Tree root);
void transParamDec(Tree root);

void transCompSt(Tree root);

void transDefList(Tree root);
void transDef(Tree root);
void transDecList(Tree root);
void transDec(Tree root);

void transStmtList(Tree root);
void transStmt(Tree root); // check the return type
void transExp(Tree root, Operand place);
void transCond(Tree root, Operand label_true, Operand label_false);
void transArgs(Tree root, Operand* argList, int index);

#endif