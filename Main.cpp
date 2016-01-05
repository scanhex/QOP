#include <algorithm>
#include <string>
#include <vector>
#include <cassert>
#include <functional>
#include <sstream>
#include <map>
#include <iostream>
#include <fstream>
#include <set>
#include <FlexLexer.h>
#include <unordered_set>
#include <unordered_map>
#include <stack>

#include "optype.hpp"
#include "value.hpp"
#include "node.hpp"
#include "parser.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc > 1)
		lexer = yyFlexLexer(new ifstream(argv[1]));
	/*
	string filename = argc > 1 ? argv[1] : "input.txt";
	ifstream cin(filename);
	string s;
	vector<string> prog;
	while (getline(cin, s))
		prog.push_back(s);
		exec(prog, null);
	*/
	parser a;
	auto prog = a.parse();
	prog->exec(null);
	system("pause");
	return 0;
}