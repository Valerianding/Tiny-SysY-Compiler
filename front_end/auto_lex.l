%{
    #include "ast.h"
    #include "bstrlib.h"
    #include <math.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "lrparser.tab.h"
    extern FILE* yyin;

    //将单个字符转为十六进制数字
    int __my_digit(char ch)
    {
    	if (ch >= '0' && ch <= '9')
    		return ch-48;
    	else if (ch >= 'A' && ch <= 'F')
    	{
    		return ch-55;
    	}
    	else if (ch >= 'a' && ch <= 'f')
    	{
    		return ch-87;
    	}
    	else
    		return -1;
    }

    //解析数字
    int __my_parse_integer(char *InputWordFromOut, int dex)
    {
    	int i = 0;
    	for(i=0;;i++)	//	计算字符串长度
    	{
    		if(InputWordFromOut[i] == '\0')
    		{
    			break;
    		}
    	}
    	int SUM = 0;
    	int Weights = 1;
    	int n;
    	if (dex == 10)
    	{
    		n = -1;
    	}
    	else if (dex == 8)
    	{
    		n = 0;
    	}
    	else
    	{
    		n = 1;
    	}

    	for (i--; i != n ; i--)
    	{
    		SUM = SUM + Weights * __my_digit(InputWordFromOut[i]);
    		Weights = Weights * dex;
    	}
    	return SUM;
    }


    int __my_parse_float(char *Input)
    {
    	float ret = atof(Input);
    	int *retint = (int*)&ret;
        return *retint;
    }

    int __my_parse_hex_float(char *Input)
    {
    	float ret;
    	int *retint;
    	char tem_value[256];
    	int lenth_float = 1;
    	int lenth_p = 1;
    	int signal_p = 1;
    	int i = 2;
    	int j;

    	tem_value[0] = '0';
    	tem_value[1] = 'x';
    	tem_value[2] = '0';
    	tem_value[3] = 0;
    	j = 2;
    	while (Input[i] != '.' && Input[i] != 'p' && Input[i] != 'P')
    	{
    		tem_value[j] = Input[i];
    		i = i + 1;
    		j = j + 1;
    	}
    	tem_value[j] = 0;
    	ret = __my_parse_integer(tem_value, 16);

    	j = 2;
    	tem_value[2] = '0';
    	tem_value[3] = 0;
    	if (Input[i] == '.')
    	{
    		i = i + 1;
    		while (Input[i] != 'p' && Input[i] != 'P')
    		{
    			lenth_float = lenth_float * 16;
    			tem_value[j] = Input[i];
    			i = i + 1;
    			j = j + 1;
    		}
    	}
    	tem_value[j] = 0;
    	ret = ret + (float)__my_parse_integer(tem_value, 16)/lenth_float;
    	i = i + 1;
    	if (Input[i] == '-')
    	{
    		signal_p = 0;
    		i = i+1;
    	}
    	else if (Input[i] == '+')
    	{
    		signal_p = 1;
    		i = i+1;
    	}


    	j = 0;
    	tem_value[0] = '0';
    	tem_value[1] = 0;
    	while (Input[i] != 0)
    	{
    		tem_value[j] = Input[i];
    		i = i + 1;
    		j = j + 1;
    	}
    	tem_value[j] = 0;
    	j = __my_parse_integer(tem_value, 10);
    	lenth_p = pow(2, j);

    	if (signal_p == 1)
    	{
    		ret = ret * lenth_p;
    	}
    	else
    	{
    		ret = ret / lenth_p;
    	}

    	retint = (int*) &ret;
        return *retint;
    }
%}

INT_DEX [1-9][0-9]*
INT_HEX 0[Xx][0-9a-fA-F]+
INT_OCT [0][0-7]*
FLOAT 	([0-9]+[\.][0-9]*|[0-9]*[\.][0-9]+)
FLOAT_HEX  (([0-9a-fA-F]*\.[0-9a-fA-F]+)|([0-9a-fA-F]+\.))

IDent [A-Za-z_][A-Za-z0-9_]*

int "int"
float "float"
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

WhiteSpace    [ \t\n\r]*
LineComment (\/\/.*)
BlockComment \/\*(.|\n)*\*\/

%%
{WhiteSpace} {}
{LineComment} {}
{BlockComment} {}

{INT_DEX} 			{ yylval.iVal = atoi(yytext); return num_int;}
{INT_HEX} 			{ yylval.iVal = __my_parse_integer(yytext, 16); return num_int;}
{INT_OCT} 			{ yylval.iVal = __my_parse_integer(yytext, 8); return num_int;}


{FLOAT}																					{ yylval.fVal = atof(yytext); return num_float;}
({FLOAT}|0|([1-9][0-9]*))[eE][-+]?(0|([1-9][0-9]*))   									{ yylval.fVal = __my_parse_float(yytext); return num_float;}
0[xX]{FLOAT_HEX}    																	{ yylval.fVal = __my_parse_float(yytext); return num_float;}
0[xX]({FLOAT_HEX}|0|([1-9a-fA-F][0-9a-fA-F]*))[pP][-+]?(0|([1-9a-fA-F][0-9a-fA-F]*))   	{ yylval.fVal = __my_parse_hex_float(yytext); return num_float;}

{int}  {
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

{float} {
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




