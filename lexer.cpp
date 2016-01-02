/* scanner for a toy Pascal-like language */

%{
/* need this for the call to atof() below */
#include <math.h>
#include <iostream>
#include <string>
#include "optype.hpp"
using namespace std;

string str;
int num; 

%}

%option noyywrap
%option c++
%option outfile="lexer.yy.cpp"         
                      
DIGIT    [0-9]
ID       [A-Za-z][_A-Za-z0-9]*

%%
{DIGIT}+    {
             	num = atoi( yytext ); 
            	return NUMBER;
			}

wh return WHILE;
"if" return IF;
ret return RETURN;

{ID} 	{
			str = string(yytext);
			return ID;
		}

"&" return AND;
"|" return OR;
"^" return XOR;
"=" return ASSIGN;
"&&" return GAND;
"||" return GOR;
"<" return LESS;
">" return GREATER;
"?" return EQUALS;
"+" return PLUS;
"-" return MINUS; 
"*" return MULT;
"/" return DIV;
"%" return MOD;
"(" return LPAREN;
")" return RPAREN;
"{" return LBRACE;
"}" return RBRACE;
"[" return LBRACKET;
"]" return RBRACKET;
"!" return NOT;
. return NEWLINE;
%%
               