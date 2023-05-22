%{
  #include "lex.yy.c"
  #include "mft.h"
  struct MFT* root;
  extern int synError;
%}
%union{
  struct MFT* node;
}

// tokens

%token <node> INT
%token <node> FLOAT
%token <node> ID
%token <node> TYPE
%token <node> COMMA
%token <node> DOT
%token <node> SEMI
%token <node> RELOP
%token <node> ASSIGNOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR NOT 
%token <node> LP RP LB RB LC RC
%token <node> IF
%token <node> ELSE
%token <node> WHILE
%token <node> STRUCT
%token <node> RETURN

// non-terminators

%type <node> Program ExtDefList ExtDef ExtDecList   //  High-level Definitions
%type <node> Specifier StructSpecifier OptTag Tag   //  Specifiers
%type <node> VarDec FunDec VarList ParamDec         //  Declarators
%type <node> CompSt StmtList Stmt                   //  Statements
%type <node> DefList Def Dec DecList                //  Local Definitions
%type <node> Exp Args                               //  Expressions

// precedence and associativity

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
// High-level Definitions
Program:            ExtDefList                              { $$ = addNode(@$.first_line, "Program", NOT_A_TOKEN, 1, $1); root = $$; }
    ; 
ExtDefList:         ExtDef ExtDefList                       { $$ = addNode(@$.first_line, "ExtDefList", NOT_A_TOKEN, 2, $1, $2); }
    |                                                       { $$ = addNode(@$.first_line, "ExtDefList", NODE_NULL, 0, 0); } 
    ; 
ExtDef:             Specifier ExtDecList SEMI               { $$ = addNode(@$.first_line, "ExtDef", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Specifier SEMI                          { $$ = addNode(@$.first_line, "ExtDef", NOT_A_TOKEN, 2, $1, $2); }
    |               Specifier FunDec CompSt                 { $$ = addNode(@$.first_line, "ExtDef", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               error SEMI                              { synError = 1; }    
    ; 
ExtDecList:         VarDec                                  { $$ = addNode(@$.first_line, "ExtDecList", NOT_A_TOKEN, 1, $1); }
    |               VarDec COMMA ExtDecList                 { $$ = addNode(@$.first_line, "ExtDecList", NOT_A_TOKEN, 3, $1, $2, $3); }
    ; 

// Specifiers
Specifier:          TYPE                                    { $$ = addNode(@$.first_line, "Specifier", NOT_A_TOKEN, 1, $1); }
    |               StructSpecifier                         { $$ = addNode(@$.first_line, "Specifier", NOT_A_TOKEN, 1, $1); }
    ; 
StructSpecifier:    STRUCT OptTag LC DefList RC             { $$ = addNode(@$.first_line, "StructSpecifier", NOT_A_TOKEN, 5, $1, $2, $3, $4, $5); }
    |               STRUCT Tag                              { $$ = addNode(@$.first_line, "StructSpecifier", NOT_A_TOKEN, 2, $1, $2); }
    ; 
OptTag:             ID                                      { $$ = addNode(@$.first_line, "OptTag", NOT_A_TOKEN, 1, $1); }
    |                                                       { $$ = addNode(@$.first_line, "OptTag", NODE_NULL, 0, 0); } 
    ; 
Tag:                ID                                      { $$ = addNode(@$.first_line, "Tag", NOT_A_TOKEN, 1, $1); }
    ; 

// Declarators
VarDec:             ID                                      { $$ = addNode(@$.first_line, "VarDec", NOT_A_TOKEN, 1, $1); }
    |               VarDec LB INT RB                        { $$ = addNode(@$.first_line, "VarDec", NOT_A_TOKEN, 4, $1, $2, $3, $4); }
    |               error RB                                { synError = 1; }    
    ; 
FunDec:             ID LP VarList RP                        { $$ = addNode(@$.first_line, "FunDec", NOT_A_TOKEN, 4, $1, $2, $3, $4); }
    |               ID LP RP                                { $$ = addNode(@$.first_line, "FunDec", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               error RP                               { synError = 1; }    
    ; 
VarList:            ParamDec COMMA VarList                  { $$ = addNode(@$.first_line, "VarList", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               ParamDec                                { $$ = addNode(@$.first_line, "VarList", NOT_A_TOKEN, 1, $1); }
    ; 
ParamDec:           Specifier VarDec                        { $$ = addNode(@$.first_line, "ParamDec", NOT_A_TOKEN, 2, $1, $2); }
    ; 
// Statements
CompSt:             LC DefList StmtList RC                  { $$ = addNode(@$.first_line, "CompSt", NOT_A_TOKEN, 4, $1, $2, $3, $4); }
    |               error RC                                { synError = 1; }
    ; 
StmtList:           Stmt StmtList                           { $$ = addNode(@$.first_line, "StmtList", NOT_A_TOKEN, 2, $1, $2); }
    |                                                       { $$ = addNode(@$.first_line, "StmtList", NODE_NULL, 0, 0); } 
    ; 
Stmt:               Exp SEMI                                { $$ = addNode(@$.first_line, "Stmt", NOT_A_TOKEN, 2, $1, $2); }
    |               CompSt                                  { $$ = addNode(@$.first_line, "Stmt", NOT_A_TOKEN, 1, $1); }
    |               RETURN Exp SEMI                         { $$ = addNode(@$.first_line, "Stmt", NOT_A_TOKEN, 3, $1, $2, $3); }    
    |               IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = addNode(@$.first_line, "Stmt", NOT_A_TOKEN, 5, $1, $2, $3, $4, $5); }
    |               IF LP Exp RP Stmt ELSE Stmt             { $$ = addNode(@$.first_line, "Stmt", NOT_A_TOKEN, 7, $1, $2, $3, $4, $5, $6, $7); }
    |               WHILE LP Exp RP Stmt                    { $$ = addNode(@$.first_line, "Stmt", NOT_A_TOKEN, 5, $1, $2, $3, $4, $5); }
    |               error SEMI                              { synError = 1; }
    ; 
// Local Definitions
DefList:            Def DefList                             { $$ = addNode(@$.first_line, "DefList", NOT_A_TOKEN, 2, $1, $2); }
    |                                                       { $$ = addNode(@$.first_line, "DefList", NODE_NULL, 0, 0); } 
    ;     
Def:                Specifier DecList SEMI                  { $$ = addNode(@$.first_line, "Def", NOT_A_TOKEN, 3, $1, $2, $3); }
    ; 
DecList:            Dec                                     { $$ = addNode(@$.first_line, "DecList", NOT_A_TOKEN, 1, $1); }
    |               Dec COMMA DecList                       { $$ = addNode(@$.first_line, "DecList", NOT_A_TOKEN, 3, $1, $2, $3); }
    ; 
Dec:                VarDec                                  { $$ = addNode(@$.first_line, "Dec", NOT_A_TOKEN, 1, $1); }
    |               VarDec ASSIGNOP Exp                     { $$ = addNode(@$.first_line, "Dec", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               error SEMI                              { synError = 1;}
    ; 
// Expressions
Exp:                Exp ASSIGNOP Exp                        { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp AND Exp                             { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp OR Exp                              { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp RELOP Exp                           { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp PLUS Exp                            { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp MINUS Exp                           { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp STAR Exp                            { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp DIV Exp                             { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               LP Exp RP                               { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               MINUS Exp                               { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 2, $1, $2); }
    |               NOT Exp                                 { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 2, $1, $2); }
    |               ID LP Args RP                           { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 4, $1, $2, $3, $4); }
    |               ID LP RP                                { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp LB Exp RB                           { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 4, $1, $2, $3, $4); }
    |               Exp DOT ID                              { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               ID                                      { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 1, $1); }
    |               INT                                     { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 1, $1); }
    |               FLOAT                                   { $$ = addNode(@$.first_line, "Exp", NOT_A_TOKEN, 1, $1); }
    ; 
Args :              Exp COMMA Args                          { $$ = addNode(@$.first_line, "Args", NOT_A_TOKEN, 3, $1, $2, $3); }
    |               Exp                                     { $$ = addNode(@$.first_line, "Args", NOT_A_TOKEN, 1, $1); }
    ; 
%%

yyerror(char* msg){
    fprintf(stderr, "Error type B at line %d: %s.\n", yylineno, msg);
}
