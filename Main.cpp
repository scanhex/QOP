#include <algorithm>
#include <cassert>
#include <sstream>

#include "value.hpp"
#include "node.hpp"
#include "parser.hpp"

using namespace std;

int main(int argc, char* argv[])
{
//	if (argc > 1)
//		lexer = yyFlexLexer(new ifstream(argv[1]));
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