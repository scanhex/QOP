#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdio> 
#include <functional>
#include <sstream>
#include <map>
#include <set>
#define null NULL
// #include "parser.cpp"

using namespace std;

const int SZ = 500000;
const int LCSZ = 32;


map<string, vector<string>> func;
set<string> built_in;

int func_call(string name, int* local);

void init_func()
{
	func["plus"] = { "p {0} + {1}\n" };
}

struct VALUE
{
	enum type
	{
		INT, STRING, DOUBLE, HZ
	};
	type t;
	void* data;
	VALUE(type t, void* data)
		: t(t), data(data)
	{}
	VALUE()
		: t(HZ), data(null)
	{}
	VALUE operator !()
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data));
		if (t == DOUBLE)
			return VALUE(t, new double(*(double*)data));
		return VALUE(HZ, null);
	}
	VALUE operator +(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data + *(int*)other.data));
	}
	VALUE operator -(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data - *(int*)other.data));
	}
	VALUE operator *(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data * *(int*)other.data));
	}
	VALUE operator /(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data / *(int*)other.data));
	}
	VALUE operator %(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data % *(int*)other.data));
	}
	VALUE operator &(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data & *(int*)other.data));
	}
	VALUE operator |(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data | *(int*)other.data));
	}
	VALUE operator <(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data < *(int*)other.data));
	}
	VALUE operator >(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data > *(int*)other.data));
	}
	VALUE operator ==(VALUE& other)
	{
		if (t == INT)
			return VALUE(t, new int(*(int*)data == *(int*)other.data));
	}
};

VALUE* arr = new VALUE[2 * SZ] + SZ;

int toint(VALUE val)
{
	return *(int*)val.data;
}

struct nd
{
	vector<nd*> ch;
	enum type
	{
		MULT, PLUS, MINUS, DIV, MOD, OR, AND, XOR, GOR, GAND, LESS, GREATER, EQUALS, COLON, NEWLINE, WHILE, NOT, BRACES, SBRACES, FBRACES, FUNC_CALL, VAR
	};
	type t;
	VALUE value;
	VALUE exec(VALUE* local);
	nd(type t, vector<nd*> ch);
	nd(VALUE val);
};

VALUE exec(vector<nd*>& nds, nd::type type, VALUE* local)
{
	if (type == nd::PLUS)
	{
		return nds[0]->exec(local) + nds[1]->exec(local);
	}
	if (type == nd::MINUS)
	{
		return nds[0]->exec(local) - nds[1]->exec(local);
	}
	if (type == nd::MULT)
	{
		return nds[0]->exec(local) * nds[1]->exec(local);
	}
	if (type == nd::DIV)
	{
		return nds[0]->exec(local) / nds[1]->exec(local);
	}
	if (type == nd::MOD)
	{
		return nds[0]->exec(local) % nds[1]->exec(local);
	}
	if (type == nd::OR)
	{
		return nds[0]->exec(local) | nds[1]->exec(local);
	}
	if (type == nd::AND)
	{
		return nds[0]->exec(local) & nds[1]->exec(local);
	}
	if (type == nd::LESS)
	{
		return nds[0]->exec(local) < nds[1]->exec(local);
	}
	if (type == nd::GREATER)
	{
		return nds[0]->exec(local) > nds[1]->exec(local);
	}
	if (type == nd::EQUALS)
	{
		return nds[0]->exec(local) == nds[1]->exec(local);
	}
	if (type == nd::SBRACES)
	{
		return local[toint(nds[0]->exec(local))];
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
nd::nd(type t, vector<nd*> ch)
	:t(t), ch(ch), value(VALUE::HZ, null)
{}
nd::nd(VALUE val)
	: value(val), t(VAR)
{}

struct parser
{
	string s;
	int cur;
	parser(string s)
		: s(s + "$"), cur(0)
	{}

	nd* parse()
	{
		return parse(0);
	}

	nd* parseLeaf()
	{
		if (s[cur] == '!')
		{
			++cur;
			return new nd(nd::NOT, { parseLeaf() });
		}
		if (s[cur] == '(')
		{
			++cur;
			nd* ans = parse();
			++cur;
			return ans;
		}
		if (s[cur] == '[')
		{
			++cur;
			nd* ans = parse();
			assert(s[cur] == ']');
			++cur;
			return new nd(nd::SBRACES, { ans });
		}
		if (s[cur] == '{')
		{
			++cur;
			nd* ans = parse();
			if (s[cur] != '}')
				cerr << "Not enough }" << endl;
			++cur;
			return new nd(nd::FBRACES, { ans });
		}
		if (isalpha(s[cur]))
		{
			string name = "";
			while (s[cur] != '(')
				name.push_back(s[cur++]);
			auto x = parseArgs();
			vector<nd*> args = parseArgs();
			args.insert(args.begin(), new nd(VALUE(VALUE::STRING, &name)));
			return new nd(nd::FUNC_CALL, args);
		}
		int x = 0;
		while (isdigit(s[cur]))
			x *= 10, x += s[cur++] - '0';
		return new nd(VALUE(VALUE::INT, new int(x)));
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


	int get_prior(string op)
	{
		if (op == "&")
			return 0;
		if (op == "|")
			return 0;
		if (op == "<")
			return 1;
		if (op == ">")
			return 1;
		if (op == "?")
			return 1;
		if (op == "+")
			return 2;
		if (op == "-")
			return 2;
		if (op == "*")
			return 3;
		if (op == "/")
			return 3;
		if (op == "%")
			return 3;
		return -1;
	}
	nd::type get_type(string op)
	{
		if (op == "&")
			return nd::AND;
		if (op == "|")
			return nd::OR;
		if (op == "<")
			return nd::LESS;
		if (op == ">")
			return nd::GREATER;
		if (op == "?")
			return nd::EQUALS;
		if (op == "+")
			return nd::PLUS;
		if (op == "-")
			return nd::MINUS;
		if (op == "*")
			return nd::MULT;
		if (op == "/")
			return nd::DIV;
		if (op == "%")
			return nd::MOD;
	}
	nd* parse(int prior)
	{
		if (prior == 4)
			return parseLeaf();
		nd* x = parse(prior + 1);
		while (get_prior(string({ s[cur] })) == prior)
		{
			++cur;
			x = new nd(get_type(string({ s[cur] })), { x, parse(prior + 1) });
		}
		return x;
	}
	vector<nd*> parseArgs1()
	{
		vector<nd*> args;
		if (s[cur] == ')')
			return args;
		args.push_back(parse());
		while (s[cur++] == ',')
			args.push_back(parse());
		--cur;
		return args;
	}
	vector<nd*> parseArgs()
	{
		if (s[cur] == '(')
		{
			++cur;
			auto ans = parseArgs1();
			++cur;
			return ans;
		}
		return parseArgs1();
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

int exec(vector<string> prog, VALUE* local);

int exec_while(vector<string> prog, string expr, VALUE* local)
{
	int res = 0;
	while (parser(expr).parse() && (res = exec(prog, local)) >= 0)
	{
	}
	if (res < 0)
		return res + 1;
	return res;
}



int exec(vector<string> prog, VALUE* local)
{
	for (int it = 0; it < prog.size(); ++it)
	{
		string s = prog[it];
		unspace(s);
		if (s[0] == 'g' && s[1] == 'c')
		{
			parser p(s.substr(2));
			char ch;
			cin >> ch;
			arr[toint(p.parse()->exec(local))] = VALUE(VALUE::INT, new int(ch));
		}
		else if (s[0] == 'g')
		{
			parser p(s.substr(1));
			int x;
			cin >> x;
			arr[toint(p.parse()->exec(local))] = VALUE(VALUE::INT, new int(x));
		}
		else if (s[0] == '[')
		{
			int p = find(s.begin(), s.end(), '=') - s.begin();
			string s1 = s.substr(1, p - 2);
			string s2 = s.substr(p + 1, s.size() - p - 1);
			parser p1(s1), p2(s2);
			int ind = toint(p1.parse()->exec(local));
			int val = toint(p2.parse()->exec(local));
			// cout << p << ' ' << s1 << ' ' << s2 << endl;
			arr[ind] = VALUE(VALUE::INT, new int(val));
		}
		else if (s[0] == '{')
		{
			int p = find(s.begin(), s.end(), '=') - s.begin();
			string s1 = s.substr(1, p - 2);
			string s2 = s.substr(p + 1, s.size() - p - 1);
			parser p1(s1), p2(s2);
			int ind = toint(p1.parse()->exec(local));
			int val = toint(p2.parse()->exec(local));
			// cout << p << ' ' << s1 << ' ' << s2 << endl;
			local[ind] = VALUE(VALUE::INT, new int(val));
		}
		else if (s[0] == 'p' && s[1] == 'c')
		{
			parser p(s.substr(2, s.size() - 1));
			cout << (char)p.parse();
		}
		else if (s[0] == 'p' && s[1] == 's')
		{
			cout << s.substr(3, s.size() - 3 - 1);
		}
		else if (s[0] == 'p')
		{
			parser p(s.substr(1, s.size() - 1));
			cout << toint(p.parse()->exec(local));
		}
		else if (s[0] == 'r')
		{
			parser p(s.substr(1));
			return toint(p.parse()->exec(local));
		}
		else if (s[0] == '/' && s[1] == '\\')
		{
			parser p(s.substr(2));
			return -toint(p.parse()->exec(local));
		}
		else if (s[0] == 'w' && s[1] == 'h')
		{
			string expr = s.substr(2);
			vector<string> p = read_block(prog, it);
			int res = exec_while(p, expr, local);
			if (res < 0)
				return res;
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
			parser(expr).parse();
		}
		else
		{
			cout << "Undefined behaviour" << endl;
		}
	}
	return 0;
}

void built_in_call(string name)
{}

int func_call(string name, VALUE* local)
{
	if (func.count(name))
	{
		return exec(func[name], local);
	}
	else
	{
		cout << "Undefined function: " << name << endl;
		return 0;
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
	exec(prog, (VALUE*)null);
	return 0;
}