#include <FlexLexer.h>
#include <iostream>
using namespace std;

extern string str;

int main()
{
	yyFlexLexer x;
	while (!cin.eof())
		x.yylex(), cout << string(str) << endl;	
	return 0;
}