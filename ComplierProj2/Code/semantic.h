#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "mft.h"
#include "symbolTable.h"

enum ErrorType{
    UNDEF_VAR = 1,         // Undefined Variable
    UNDEF_FUNC,            // Undefined Function
    REDEF_VAR,             // Redefined Variable
    REDEF_FUNC,            // Redefined Function
    TYPE_MISMATCH_ASSIGN,  // Type mismatchedfor assignment.
    LEFT_VAR_ASSIGN,       // The left-hand side of an assignment must be a variable.
    TYPE_MISMATCH_OP,      // Type mismatched for operands.
    TYPE_MISMATCH_RETURN,  // Type mismatched for return.
    FUNC_AGRC_MISMATCH,    // Function is not applicable for arguments
    NOT_A_ARRAY,           // Variable is not a Array
    NOT_A_FUNC,            // Variable is not a Function
    NOT_A_INT,             // Variable is not a Integer
    ILLEGAL_USE_DOT,       // Illegal use of "."
    NONEXISTFIELD,         // Non-existentfield
    REDEF_FEILD,           // Redefined field
    REDEF_STRUCT,          // Duplicated name
    UNDEF_STRUCT,          // Undefined structure
    OTHER                  // Other errors
};
typedef struct MFT* Tree;
void Program(Tree root); // the entry of semantic analyze
void ExtDefList(Tree root);
void ExtDef(Tree root);

Type Specifier(Tree root);
Type StructSpecifier(Tree root); // insert an item(structure type defination)

void ExtDecList(Tree root, Type type);
TableItem VarDec(Tree root, Type type, int flag); // insert an item(varity defination)

void FunDec(Tree root, Type type); // insert an item(function defination)
FieldList VarList(Tree root);
FieldList ParamDec(Tree root);

void CompSt(Tree root, Type retType);

FieldList DefList(Tree root, int flag);
FieldList Def(Tree root, int flag);
FieldList DecList(Tree root, Type type, int flag);
FieldList Dec(Tree root, Type type, int flag);

void StmtList(Tree root, Type retType);
void Stmt(Tree root, Type retType); // check the return type
Type Exp(Tree root);
void Args(Tree root, TableItem funcInfo);

static inline void printError(enum ErrorType e, int line, char *msg){
    printf("Error type %d at Line %d: %s\n", e, line, msg);
}

#endif