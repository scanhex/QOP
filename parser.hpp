#pragma once

#include <cassert>
#include <FlexLexer.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <fstream>

#include "defines.hpp"
#include "value.hpp"
#include "node.hpp"
#include "builtins.hpp"

using std::vector;
using std::ifstream;


extern yyFlexLexer lexer;
extern string str;
extern int num, yyline;

class parser
{
private:
	unordered_set<string> func_parsed;

	bool func_exists(string name);

	void fail(string s);


	nd* parse_leaf();

	string curs;
	void move_next();

	op_type get_op_type(string op);

	int get_prior(op_type op);

	nd* parse(int prior);
	vector<nd*> parse_args();
public:
	parser();
	parser(string name);
	nd* parse();
};