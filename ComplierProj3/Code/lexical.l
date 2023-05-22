%{
  #include "mft.h" 
  #include "syntax.tab.h"
  int yycolumn = 1;
  extern unsigned lexError;
  #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
INT 0|[1-9][0-9]*|0[0-7]+|0(x|X)[0-9a-fA-F]+
FLOAT ([0-9]+\.[0-9]+)|((([0-9]*\.[0-9]+)|([0-9]+\.))(E|e)(\+|\-)?[0-9]+)
ID [a-zA-Z_][a-zA-Z0-9_]*
%%
{SEMI} { yylval.node = addLeaf(yylineno, "SEMT", TOKEN_OTHERS, yytext); return SEMI;}
{COMMA} { yylval.node = addLeaf(yylineno, "COMMA", TOKEN_OTHERS, yytext); yylval.node = addLeaf(yylineno, "SEMT", TOKEN_OTHERS, yytext); return COMMA;}
{ASSIGNOP} { yylval.node = addLeaf(yylineno, "ASSIGNOP", TOKEN_OTHERS, yytext); return ASSIGNOP;}
{RELOP} { yylval.node = addLeaf(yylineno, "RELOP", TOKEN_OTHERS, yytext); return RELOP;}
{PLUS} { yylval.node = addLeaf(yylineno, "PLUS", TOKEN_OTHERS, yytext); return PLUS;}
{MINUS} { yylval.node = addLeaf(yylineno, "MINUS", TOKEN_OTHERS, yytext); return MINUS;}
{STAR} { yylval.node = addLeaf(yylineno, "STAR", TOKEN_OTHERS, yytext); return STAR;}
{DIV} { yylval.node = addLeaf(yylineno, "DIV", TOKEN_OTHERS, yytext); return DIV;}
{AND} { yylval.node = addLeaf(yylineno, "AND", TOKEN_OTHERS, yytext); return AND;}
{OR} { yylval.node = addLeaf(yylineno, "OR", TOKEN_OTHERS, yytext); return OR;}
{DOT} { yylval.node = addLeaf(yylineno, "DOT", TOKEN_OTHERS, yytext); return DOT;}
{NOT} { yylval.node = addLeaf(yylineno, "NOT", TOKEN_OTHERS, yytext); return NOT;}
{TYPE} { yylval.node = addLeaf(yylineno, "TYPE", TOKEN_TYPE, yytext); return TYPE;}
{LP} { yylval.node = addLeaf(yylineno, "LP", TOKEN_OTHERS, yytext); return LP;}
{RP} { yylval.node = addLeaf(yylineno, "RP", TOKEN_OTHERS, yytext); return RP;}
{LB} { yylval.node = addLeaf(yylineno, "LB", TOKEN_OTHERS, yytext); return LB;}
{RB} { yylval.node = addLeaf(yylineno, "RB", TOKEN_OTHERS, yytext); return RB;}
{LC} { yylval.node = addLeaf(yylineno, "LC", TOKEN_OTHERS, yytext); return LC;}
{RC} { yylval.node = addLeaf(yylineno, "RC", TOKEN_OTHERS, yytext); return RC;}
{STRUCT} { yylval.node = addLeaf(yylineno, "STRUCT", TOKEN_OTHERS, yytext); return STRUCT;}
{RETURN} { yylval.node = addLeaf(yylineno, "RETURN", TOKEN_OTHERS, yytext); return RETURN;}
{IF} { yylval.node = addLeaf(yylineno, "IF", TOKEN_OTHERS, yytext); return IF;}
{ELSE} { yylval.node = addLeaf(yylineno, "ELSE", TOKEN_OTHERS, yytext); return ELSE;}
{WHILE} { yylval.node = addLeaf(yylineno, "WHILE", TOKEN_OTHERS, yytext); return WHILE;}
{INT} {
  yylval.node = addLeaf(yylineno, "INT", TOKEN_INT, yytext); return INT;
}
{FLOAT} {
  yylval.node = addLeaf(yylineno, "FLOAT", TOKEN_FLOAT, yytext); return FLOAT;
}
{ID} {
  yylval.node = addLeaf(yylineno, "ID", TOKEN_ID, yytext); return ID;
}
[ \t]+ { ;}
\n+ { yycolumn = 1; yylineno++;}
. { lexError = 1; printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}
%%