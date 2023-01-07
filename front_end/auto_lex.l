%{
    #include "ast.h"
    #include "bstrlib.h"
    #include <stdlib.h>
    #include "lrparser.tab.h"
    extern FILE* yyin;
%}
num_int 0|([1-9]([0-9]*)?)
num_float [0-9]*(\.[0-9]+)?

IDent [A-Za-z_][A-Za-z0-9_]*

INT "int"
VOID "void"
CONST "const"
IF "if"
ELSE "else"
WHILE "while"
BREAK "break"
CONTINUE "continue"
RETURN "return"
ADD "+"
SUB "-"
MUL "*"
DIV "/"
MODULO "%"
LESS "<"
LESSEQ "<="
GREAT ">"
GREATEQ ">="
NOTEQ "!="
EQ "=="
NOT "!"
AND "&&"
OR "||"
ASSIGN "="

LPAR "{"
RPAR "}"
LBRACKET "("
RBRACKET ")"
LSQUARE "["
RSQUARE "]"
COMMA ","
SEMICOLON ";"

FLOAT "float"

WhiteSpace    [ \t\n\r]*
LineComment (\/\/.*)
BlockComment \/\*(.|\n)*\*\/

%%
{WhiteSpace} {}
{LineComment} {}
{BlockComment} {}

{num_int} {
    yylval.iVal=atoi(yytext);
    return num_int;
}

{num_float} {
    yylval.fVal=atof(yytext);
    return num_float;
}

{INT}  {
    return INT;
}

{VOID}  {
    return VOID;
}

{CONST} {
    return CONST;
}

{IF} {
    return IF;
}

{ELSE} {
    return ELSE;
}

{WHILE} {
    return WHILE;
}

{BREAK} {
    return BREAK;
}

{CONTINUE} {
    return CONTINUE;
}

{RETURN} {
    return RETURN;
}

{LESSEQ} {
    return LESSEQ;
}

{GREATEQ} {
    return GREATEQ;
}

{NOT} {
   return '!';
}

{NOTEQ} {
    return NOTEQ;
}

{EQ} {
    return EQ;
}

{AND} {
    return AND;
}

{OR} {
    return OR;
}

{ASSIGN} {
    return ASSIGN;
}

{LPAR} {
    return LPAR;
}

{RPAR} {
    return RPAR;
}

{LBRACKET} {
    return LBRACKET;
}

{RBRACKET} {
    return RBRACKET;
}

{LSQUARE} {
    return LSQUARE;
}

{RSQUARE} {
    return RSQUARE;
}

{COMMA} {
    return COMMA;
}

{SEMICOLON} {
    return SEMICOLON;
}

{FLOAT} {
    return FLOAT;
}

{IDent} {
    yylval.sVal = bfromcstr(yytext);
    return IDent;
}

{GREAT} {
   return GREAT;
}

{LESS} {
    return LESS;
}

{ADD} {
    return '+';
}

{SUB} {
    return '-';
}

{MUL} {
    return '*';
}

{DIV} {
    return '/';
}

{MODULO} {
    return '%';
}

%%

int yywrap() {
    return 1;
}




