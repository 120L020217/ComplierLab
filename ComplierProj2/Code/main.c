#include "syntax.tab.h"
#include "mft.h"
#include "semantic.h"

extern struct MFT* root;
extern Table table;
extern int yyparse();
extern void yyrestart(FILE*);
unsigned lexError = 0;
unsigned synError = 0;

int main(int argc, char** argv) {
    table = newTable();
    assert(table);
    if (argc <= 1) {
        return 1;
    }

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
        // printTable(table);
    }
    deleteTable(table);
    release(root);
    return 0;
}
