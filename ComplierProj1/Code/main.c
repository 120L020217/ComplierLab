#include "syntax.tab.h"
#include "mft.h"

extern struct MFT* root;
extern int yyparse();
extern void yyrestart(FILE*);
unsigned lexError = 0;
unsigned synError = 0;

int main(int argc, char** argv) {
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
    if (!lexError && !synError){
        preorder(root, 1);
    }
    release(root);
    return 0;
}