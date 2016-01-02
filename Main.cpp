#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <functional>
#include <sstream>
#include <map>
#include <set>
#include <FlexLexer.h>
#include "optype.hpp"
#define null NULL
// #include "parser.cpp"

using namespace std;

const int SZ = 500000;
const int LCSZ = 32;


map<string, vector<string>> func;
set<string> built_in;

void init_func()
{
	func["plus"] = { "p {0} + {1}\n" };
}

struct VALUE
{
	enum type
	{
		INT, STRING, DOUBLE, VOID
	};
	type t;
	union
	{
		void* data;
		int iv;
		double dv;
		char cv;
	};
	VALUE(type t, void* data)
		: t(t), data(data)
	{}
	VALUE(int iv)
		: t(INT), iv(iv)
	{}
	VALUE(double dv)
		: t(DOUBLE), dv(dv)
	{}
	VALUE()
		: t(VOID), data(null)
	{}
	VALUE operator !()
	{
		if (t == INT)
			return VALUE((int)!iv);
		if (t == DOUBLE)
			return VALUE((double)!dv);
		return VALUE(VOID, null);
	}
	VALUE operator +(VALUE other)
	{
		if (t == INT)
			return VALUE(iv + other.iv);
		return VALUE(VOID, null);
	}
	VALUE operator -(VALUE other)
	{
		if (t == INT)
			return VALUE(iv - other.iv);
	}
	VALUE operator *(VALUE other)
	{
		if (t == INT)
			return VALUE(iv * other.iv);
	}
	VALUE operator /(VALUE other)
	{
		if (t == INT)
			return VALUE(iv / other.iv);
	}
	VALUE operator %(VALUE other)
	{
		if (t == INT)
			return VALUE(iv % other.iv);
	}
	VALUE operator &(VALUE other)
	{
		if (t == INT)
			return VALUE(iv & other.iv);
	}
	VALUE operator |(VALUE other)
	{
		if (t == INT)
			return VALUE(iv | other.iv);
	}
	VALUE operator ^(VALUE other)
	{
		if (t == INT)
			return VALUE(iv ^ other.iv);
	}
	VALUE operator <(VALUE other)
	{
		if (t == INT)
			return VALUE(iv < other.iv);
	}
	VALUE operator >(VALUE other)
	{
		if (t == INT)
			return VALUE(iv > other.iv);
	}
	VALUE operator ==(VALUE other)
	{
		if (t == INT)
			return VALUE(iv == other.iv);
	}
	explicit operator bool()
	{
		if (t == INT)
			return (bool)t;
		if (t == DOUBLE)
			return (bool)t;
		assert(false && "Type is not int or double");
	}
};

VALUE func_call(string name, VALUE* local);

VALUE* arr = new VALUE[2 * SZ] + SZ;

struct nd
{
	vector<nd*> ch;
	
	op_type t;
	VALUE value;
	VALUE exec(VALUE* local);
	nd(op_type t, vector<nd*> ch);
	nd(VALUE val);
};

VALUE exec(nd* nd, VALUE* local)
{
	op_type op = nd->t;
	if (op == PLUS)
	{
		return nds[0]->exec(local) + nds[1]->exec(local);
	}
	if (op == MINUS)
	{
		return nds[0]->exec(local) - nds[1]->exec(local);
	}
	if (op == MULT)
	{
		return nds[0]->exec(local) * nds[1]->exec(local);
	}
	if (op == DIV)
	{
		return nds[0]->exec(local) / nds[1]->exec(local);
	}
	if (op == MOD)
	{
		return nds[0]->exec(local) % nds[1]->exec(local);
	}
	if (op == OR)
	{
		return nds[0]->exec(local) | nds[1]->exec(local);
	}
	if (op == AND)
	{
		return nds[0]->exec(local) & nds[1]->exec(local);
	}
	if (op == XOR)
	{
		return nds[0]->exec(local) ^ nds[1]->exec(local);
	}
	if (op == GAND)
	{
		VALUE x = nds[0]->exec(local);
		if ((bool)x == false)
			return false;
		return nds[1]->exec(local);
	}
	if (op == GOR)
	{
		VALUE x = nds[0]->exec(local);
		if ((bool)x == true)
			return true;
		return nds[1]->exec(local);
	}
	if (op == LESS)
	{
		return nds[0]->exec(local) < nds[1]->exec(local);
	}
	if (op == GREATER)
	{
		return nds[0]->exec(local) > nds[1]->exec(local);
	}
	if (op == EQUALS)
	{
		return nds[0]->exec(local) == nds[1]->exec(local);
	}
	if (op == BRACES)
	{
		return local[nds[0]->exec(local).iv];
	}
	if (op == BRACKETS)
	{
		return arr[nds[0]->exec(local).iv];
	}
	if (op == FUNC_CALL)
	{
		VALUE* vals = new VALUE[LCSZ + nds.size() - 1];
		for (int i = 1; i < nds.size(); ++i)
			vals[LCSZ + i - 1] = nds[i]->exec(local);
		return func_call(*(string*)(nds[0]->value.data), vals + LCSZ);
	}
	if (op == NEWLINE)
	{
		exec({ nds[0] }, local);
		exec({ nds[1] }, local);
	}
	if (op == WHILE)
	{

	}
}

VALUE nd::exec(VALUE* local)
{
	if (t == VAR)
	{
		return value;
	}
	else
	{
		return ::exec(ch, t, local);
	}
}
nd::nd(op_type t, vector<nd*> ch)
	:t(t), ch(ch), value(VALUE::VOID, null)
{}
nd::nd(VALUE val)
	: value(val), t(VAR)
{}

yyFlexLexer lexer;
extern string str;
extern int num;

struct parser
{	
	int curch;
	parser()
		: curch(0)
	{}

	parser(string s)
		: curch(0)
	{}

	nd* parse()
	{
		return parse(0);
	}

	string curs;

	op_type get_op_type(string op)
	{
		if (op == "&")
			return AND;
		if (op == "|")
			return OR;
		if (op == "^")
			return XOR;
		if (op == "=")
			return ASSIGN;
		if (op == "wh")
			return WHILE;
		if (op == "if")
			return IF;
		if (op == "&&")
			return GAND;
		if (op == "||")
			return GOR;
		if (op == "<")
			return LESS;
		if (op == ">")
			return GREATER;
		if (op == "?")
			return EQUALS;
		if (op == "+")
			return PLUS;
		if (op == "-")
			return MINUS;
		if (op == "*")
			return MULT;
		if (op == "/")
			return DIV;
		if (op == "%")
			return MOD;
		return XOR;
	}

	map<string, op_type> op_map;

	void init_op_map()
	{
		/*op_map = {
			{ "&", AND },
			{ "|", OR },
			{ "^", XOR },
			{ "=", ASSIGN },
			{ "wh", WHILE},
			{ "if", IF },
			{ "&&", GAND },
			{ "||", GOR },
			{ "<", LESS },
			{ ">", GREATER },
			{ "?", EQUALS },
			{ "+", PLUS },
			{ "-", MINUS },
			{ "*", MULT },
			{ "/", DIV },
			{ "%", MOD },
			{ "(", LPAREN },
			{ ")", RPAREN },
			{ "{", LBRACE },
			{ "}", RBRACE },
			{ "[", LBRACKET },
			{ "]", RBRACKET }
		};*/
	}

	op_type next_token()
	{
		auto ans = (op_type)lexer.yylex();
		return ans;
	}

	nd* parseLeaf()
	{
		op_type op = next_token();
		if (op == NOT)
		{
			return new nd(op, { parseLeaf() });
		}
		if (op == LPAREN)
		{
			nd* ans = parse();
			assert(next_token() == RPAREN);
			return ans;
		}
		if (op == LBRACKET)
		{
			nd* ans = parse();
			assert(next_token() == RBRACKET);
			return new nd(op, { ans });
		}
		if (op == LBRACE)
		{
			nd* ans = parse();
			assert(next_token() == RBRACE);
			return new nd(op, { ans });
		}
		if (op == FUNC_CALL)
		{
			string name = str;
			vector<nd*> args = parseArgs();
			args.insert(args.begin(), new nd(VALUE(VALUE::STRING, new string(name))));
			for (int i = 0; i < 10; ++i)
				args.push_back(new nd(VALUE(VALUE::VOID, null)));
			return new nd(op, args);
		}
		if (op == WHILE)
		{
			nd* expr = parse();
			assert(next_token() == LBRACE);
			nd* body = parse();
			assert(next_token() == RBRACE);
			return new nd(op, { expr, body });
		}
		assert(op == NUMBER);
		int x = num;
		return new nd(VALUE(x));
	}

	/*nd* parseMult()
	{
	int x = parseLeaf();
	while (s[cur] == '*' || s[cur] == '/')
	if (s[cur++] == '*')
	x *= parseInt();
	else
	x /= parseInt();
	return x;
	}*/


	int get_prior(op_type op)
	{
		if (op == NEWLINE)
			return 0;
		if (op == AND)
			return 1;
		if (op == OR)
			return 1;
		if (op == XOR)
			return 1;
		if (op == GAND)
			return 2;
		if (op == GOR)
			return 2;
		if (op == ASSIGN)
			return 3;
		if (op == LESS)
			return 4;
		if (op == GREATER)
			return 4;
		if (op == EQUALS)
			return 4;
		if (op == PLUS)
			return 5;
		if (op == MINUS)
			return 5;
		if (op == MULT)
			return 6;
		if (op == DIV)
			return 6;
		if (op == MOD)
			return 6;
		return -1;
	}
	nd* parse(int prior)
	{
		if (prior == 7)
			return parseLeaf();
		nd* x = parse(prior + 1);
		op_type tok;
		while (get_prior(tok = next_token()) == prior)
		{
			x = new nd(tok, { x, parse(prior + 1) });
		}
		return x;
	}
	
	vector<nd*> parseArgs()
	{
		assert(next_token() == LPAREN);
		vector<nd*> args;
		op_type tok = next_token();
		if (tok == RPAREN)
			return args;
		args.push_back(parse());
		while ((tok = next_token()) == COLON)
			args.push_back(parse());
		assert(tok == RPAREN);
		return args;
	}
};

void unspace(string& s)
{
	int p = 0;
	int x = 0;
	for (int i = 0; i < s.size(); ++i)
	{
		if (s[i] != ' ' && s[i] != '\t' || x)
			s[p++] = s[i];
		if (s[i] == '"')
			x ^= 1;
	}
	s.resize(p);
}

vector<string> read_block(vector<string> prog, int it)
{
	++it;
	int bal = 1;
	string s = prog[it];
	unspace(s);
	assert(s.size() == 1 && s[0] == '{');
	vector<string> ans;
	do
	{
		++it;
		s = prog[it];
		unspace(s);
		if (s.size() == 1 && s[0] == '{')
			++bal;
		if (s.size() == 1 && s[0] == '}')
			--bal;
		ans.push_back(s);
	} while (bal > 0);
	ans.pop_back();
	return ans;
}

VALUE exec(vector<string> prog, VALUE* local);

int exec_while(vector<string> prog, string expr, VALUE* local)
{
	int res = 0;
	while (parser(expr).parse()->exec(local).iv && (res = exec(prog, local).iv) >= 0)
	{
	}
	if (res < 0)
		return res + 1;
	return res;
}

ostream& operator <<(ostream& o, VALUE v)
{
	if (v.t == VALUE::INT)
		return o << v.iv;
	else
		return o;
}

VALUE exec(vector<string> prog, VALUE* local)
{
	/*if (local)
	{
		for (int i = 0; i < 10; ++i)
		{
			cout << local[i] << ' ';
		}
		cout << endl;
	}*/
	for (int it = 0; it < prog.size(); ++it)
	{
		string s = prog[it];
		unspace(s);
		if (s[0] == 'g' && s[1] == 'c')
		{
			parser p(s.substr(2));
			char ch;
			cin >> ch;
			arr[p.parse()->exec(local).iv] = VALUE(ch);
		}
		else if (s[0] == 'g')
		{
			parser p(s.substr(1));
			int x;
			cin >> x;
			arr[p.parse()->exec(local).iv] = VALUE(x);
		}
		else if (s[0] == '[')
		{
			int p = find(s.begin(), s.end(), '=') - s.begin();
			string s1 = s.substr(1, p - 2);
			string s2 = s.substr(p + 1, s.size() - p - 1);
			parser p1(s1), p2(s2);
			int ind = p1.parse()->exec(local).iv;
			int val = p2.parse()->exec(local).iv;
			// cout << p << ' ' << s1 << ' ' << s2 << endl;
			arr[ind] = VALUE(val);
		}
		else if (s[0] == '{')
		{
			int p = find(s.begin(), s.end(), '=') - s.begin();
			string s1 = s.substr(1, p - 2);
			string s2 = s.substr(p + 1, s.size() - p - 1);
			parser p1(s1), p2(s2);
			int ind = p1.parse()->exec(local).iv;
			int val = p2.parse()->exec(local).iv;
			// cout << p << ' ' << s1 << ' ' << s2 << endl;
			local[ind] = VALUE(val);
		}
		else if (s[0] == 'p' && s[1] == 'c')
		{
			parser p(s.substr(2, s.size() - 1));
			cout << (char)p.parse()->exec(local).iv;
		}
		else if (s[0] == 'p' && s[1] == 's')
		{
			cout << s.substr(3, s.size() - 3 - 1);
		}
		else if (s[0] == 'p')
		{
			parser p(s.substr(1));
			auto x = p.parse();
			cout << x->exec(local).iv;
		}
		else if (s[0] == 'r')
		{
			parser p(s.substr(1));
			return p.parse()->exec(local);
		}
		else if (s[0] == '/' && s[1] == '\\')
		{
			parser p(s.substr(2)); 
			VALUE ans = p.parse()->exec(local);
			return VALUE(-ans.iv);
		}
		else if (s[0] == 'w' && s[1] == 'h')
		{
			string expr = s.substr(2);
			vector<string> p = read_block(prog, it);
			int res = exec_while(p, expr, local);
			if (res < 0)
				return VALUE(res);
			it += p.size() + 2;
		}
		else if (s[0] == 'd' && s[1] == 'f')
		{
			string name = s.substr(2);
			vector<string> p = read_block(prog, it);
			func[name] = p;
			it += p.size() + 2;
		}
		else if (s[0] == 'q')
		{
			exit(0);
		}
		else if (s[0] == 'c' && s[1] == 'l')
		{
			string expr = s.substr(2);
			parser(expr).parse()->exec(local);
		}
		else
		{
			cout << "Undefined behaviour" << endl;
		}
	}
	return VALUE(0);
}

void built_in_call(string name)
{}

VALUE func_call(string name, VALUE* local)
{
	if (func.count(name))
	{
		return exec(func[name], local);
	}
	else
	{
		cout << "Undefined function: " << name << endl;
		return VALUE(VALUE::VOID, null);
	}
}

int main(int argc, char* argv[])
{
	string filename = argc > 1 ? argv[1] : "input.txt";
	ifstream cin(filename);
	//freopen("drill.in", "r", stdin);
	//freopen("drill.out", "w", stdout);
	string s;
	vector<string> prog;
	while (getline(cin, s))
		prog.push_back(s);
	exec(prog, null);
	return 0;
}