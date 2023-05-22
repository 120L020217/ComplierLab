#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define HASHSIZE 0x3fff
#define STACKSIZE 10
typedef enum _Kind{
    BASIC,
    ARRAY,
    STRUCTURE,
    FUNCTION
}Kind;
typedef enum _BasicType{
    TYPE_INT,
    TYPE_FLOAT
}BasicType;
typedef struct _Type* Type;
typedef struct _FieldList* FieldList;
typedef struct _TableItem* TableItem;
typedef struct _Table* Table;
struct _Type{
    Kind kind;
    union 
    {       
        BasicType basic;
        struct {
            Type eleType;
            int size;
        }array;
        struct {
            char *name;
            int num;
            FieldList field; // StructField needs a link structure
        }structure;
        struct {
            int argc;
            Type retType;
            FieldList field;
        }func;
    }u;

    bool rightValueFlag; // 1:rightvalue 0:leftvalue
};
struct _FieldList{
    char *name;
    Type fieldType;
    FieldList next;
};
struct _TableItem{
    int depth;
    char *name;
    Type type;
    TableItem hashNext;
    TableItem stackNext;
};
struct _Table{
    TableItem hash[HASHSIZE];
    TableItem stack[STACKSIZE];
    int top; /*the current top of the stack
            indicate the current scope*/
    int anonymity; // name the anonymous structure 
};
/*OOP思想，封装一些方法，在遍历语法树，建立符号表时调用*/
// Type
Type newType(Kind kind, int argc, ...);
Type copyType(Type src);
void deleteType(Type type);
bool checkType(Type type1, Type type2);
void printType(Type type);

// FieldList
FieldList newField(char * name, Type type, FieldList next);
FieldList copyField(FieldList src);
void deleteField(FieldList field);
int countField(FieldList field);
bool isRedifined(FieldList field);
Type containField(FieldList field, char *name);
void printField(FieldList field);

// TableItem
TableItem newTableItem(int depth, char* name, Type type, TableItem hn, TableItem sn);
void deleteItem(TableItem item);
void printItem(TableItem item);

// SymbolTable
Table newTable();
void deleteTable(Table table);
TableItem getItem(Table table, const char* name);
bool isdefined(Table table, TableItem item);
void addItem(Table table, TableItem item);
void removeItem(Table table, TableItem item);
void clearStackTop(Table table);
void printTable(Table table);

static inline char * newString(const char* s){
    if (s == NULL) return NULL;
    int len = strlen(s) + 1;
    char *new = (char*)malloc(sizeof(char) * len);
    assert(new != NULL);
    strncpy(new, s, len);
    return new;
}
static inline unsigned int hashCode(const char * name){
    unsigned int val = 0, i;
    for(; *name; ++name){
        val = (val << 2) + *name;
        if (i = val & ~HASHSIZE)
            val = (val ^ (i >> 12)) & HASHSIZE;
    }
    return val;
}

#endif