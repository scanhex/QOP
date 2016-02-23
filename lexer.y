/* scanner for a toy Pascal-like language */

%{
/* need this for the call to atof() below */
#include <math.h>
#include <iostream>
#include <string>
#include "optype.hpp"
using namespace std;

string str;
int num, yyline;

%}

%option noyywrap
%option c++
%option outfile="lexer.yy.cpp"         
                      
DIGIT    [0-9]
ID       @?[A-Za-z][_A-Za-z0-9]*
POINT '.'
TRAIL   [ \t]*

%%
{DIGIT}+{TRAIL}    {
             	num = atoi( yytext ); 
            	return NUMBER;
			}

{POINT}{TRAIL}  {
            num = yytext[1];
            return NUMBER;
        }

wh{TRAIL} return WHILE;
"if"{TRAIL} return IF;
"\n"{TRAIL}"else"{TRAIL} {
            ++yyline;
            return ELSE;
         }
def{TRAIL} return FUNC_DEF;
ret{TRAIL} return RETURN;

{ID}{TRAIL} 	{
			str = string(yytext);
			while (str[str.size() - 1] == ' ' || str[str.size() - 1] == '\t')
			    str.pop_back();
			return VAR;
		}
"&"{TRAIL} return AND;
"|"{TRAIL} return OR;
"^"{TRAIL} return XOR;
"="{TRAIL} return ASSIGN;
"&&"{TRAIL} return GAND;
"||"{TRAIL} return GOR;
"<"{TRAIL} return LESS;
">"{TRAIL} return GREATER;
"<="{TRAIL} return LEQ;
">="{TRAIL} return GREQ;
"=="{TRAIL} return EQUALS;
"!="{TRAIL} return NEQUALS;
"+"{TRAIL} return PLUS;
"-"{TRAIL} return MINUS;
"*"{TRAIL} return MULT;
"/"{TRAIL} return DIV;
"%"{TRAIL} return MOD;
"("{TRAIL} return LPAREN;
")"{TRAIL} return RPAREN;
"{"{TRAIL} return LBRACE;
"}"{TRAIL} return RBRACE;
"["{TRAIL} return LBRACKET;
"]"{TRAIL} return RBRACKET;
"!"{TRAIL} return NOT;
","{TRAIL} return COMMA;
("\n"{TRAIL})+ 	{
			++yyline;
			return NEWLINE;
		}
<<EOF>> return NIL;
. return NIL;
%%
               