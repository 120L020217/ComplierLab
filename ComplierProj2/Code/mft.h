#ifndef MFT_H
#define MFT_H

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
enum TYPE{
    TOKEN_TYPE,
    TOKEN_ID, 
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_OTHERS,
    NOT_A_TOKEN,
    NODE_NULL
};
struct MFT{
    int line; //number of line where the node is located
    char* name; // name of node
    enum TYPE type; // type of node(terminator or non-terminator)(for printer)
    union{ // value of node(if they have)
        int i;
        float f;
        char* id;
    };

    struct MFT* child; // the first child of the node
    struct MFT* brother; // the next brother of the node
};
int convert2binary(char* text);
// insert node(has children)
struct MFT* addNode(int line, char* name, enum TYPE type, int argc, ...);
// insert leaf(has not children)
struct MFT* addLeaf(int line, char* name, enum TYPE type, char* val);
// release memory
void release(struct MFT* root);
void preorder(struct MFT* root, int layer);

#endif