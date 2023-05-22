#include "syntax.tab.h"
#include "mft.h"
#include "semantic.h"
#include "inter.h"

extern struct MFT* root;
extern Table table;
extern InterCodes ics;
extern int yyparse();
extern void yyrestart(FILE*);
unsigned lexError = 0;
unsigned synError = 0;

int main(int argc, char** argv) {
    if (argc <= 1) {
        return 1;
    }

    table = newTable();
    assert(table);
    addItem(table, newTableItem(0, "read", newType(FUNCTION, 3, 0, newType(BASIC, 1, TYPE_INT), NULL), NULL, NULL));
    addItem(table, newTableItem(0, "write", newType(FUNCTION, 3, 1, newType(BASIC, 1, TYPE_INT), newField("des", newType(BASIC, 1, TYPE_INT), NULL)), NULL, NULL));

    ics = newInterCodes();
    assert(ics);

    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }

    yyrestart(f);
    yyparse();
    
    if (!lexError && !synError){ // with no syntax errors
        // preorder(root, 1); // for lab1
        Program(root); // for lab2
        // printTable(table); // debug for lab2
        transProgram(root); // for lab3
        // printf("test\n");
        if (argc == 3){
            FILE* fo = fopen(argv[2], "w");
            if (!f){
                perror(argv[2]);
                return 1;
            }
            fprintInterCodes(ics, fo);
            fclose(fo);
        }
    }
    // deleteInterCodes(ics);
    deleteTable(table);
    release(root);
    return 0;
}
