#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdio> 
#include <sstream>
#include <map>
#include <set>
#define null NULL
// #include "parser.cpp"

using namespace std;

const int SZ = 500000;
const int LCSZ = 32;

int* arr = new int[2 * SZ] + SZ;

map<string, vector<string>> func;
set<string> built_in;

int func_call(string name, int* local);

void init_func()
{
	func["plus"] = { "p {0} + {1}\n" };
}

struct parser
{
	string s;
	int cur;
	int* local;
	parser(string s, int local[])
		: s(s + "$"), cur(0), local(local)
	{}

	int parseInt()
	{
		if (s[cur] == '!')
		{
			++cur;
			return !parseInt();
		}
		if (s[cur] == '(')
		{
			++cur;
			int ans = parse();
			++cur;
			return ans;
		}
		if (s[cur] == '[')
		{
			++cur;
			int ind = parse();
			assert(s[cur] == ']');
			++cur;
			if (ind < -SZ || ind >= SZ) return (rand() << 16) + rand();
			return arr[ind];
		}
		if (s[cur] == '{')
		{
			++cur;
			int ind = parse();
			if (s[cur] != '}')
				cerr << "Not enough }" << endl;
			++cur;
			if (ind < -LCSZ || ind >= LCSZ) return (rand() << 16) + rand();
			return local[ind];
		}
		if ('a' <= s[cur] && s[cur] <= 'z' || 'A' <= s[cur] && s[cur] <= 'Z')
		{
			string name = "";
			while (s[cur] != '(')
				name.push_back(s[cur++]);
			return func_call(name, parseArgs());
		}
		int x = 0;
		while ('0' <= s[cur] && s[cur] <= '9')
			x *= 10, x += s[cur++] - '0';
		return x;
	}

	int parseMult()
	{
		int x = parseInt();
		while (s[cur] == '*' || s[cur] == '/')
			if (s[cur++] == '*')
				x *= parseInt();
			else
				x /= parseInt();
		return x;
	}
	int parseSum()
	{
		int x = parseMult();
		while (s[cur] == '+' || s[cur] == '-')
			if (s[cur++] == '+')
				x += parseMult();
			else
				x -= parseMult();
		return x;
	}

	int parseComp()
	{
		int x = parseSum();
		if (s[cur] == '<' || s[cur] == '>' || s[cur] == '?')
		{
			if (s[cur] == '<')
				return ++cur, x < parseSum();
			if (s[cur] == '>')
				return ++cur, x > parseSum();
			if (s[cur] == '?')
				return ++cur, x == parseSum();
		}
		return x;
	}

	int parseAnd()
	{
		int x = parseComp();
		while (s[cur] == '&')
		{
			++cur, x &= parseComp();
		}
		return x;
	}

	int parseOr()
	{
		int x = parseAnd();
		while (s[cur] == '|')
			++cur, x |= parseAnd();
		return x;
	}

	int parse()
	{
		return parseOr();
	}

	int* parseArgs1()
	{
		int* args = new int[LCSZ * 2] + LCSZ;
		if (s[cur] == ')')
			return args;
		args[0] = parse();
		for (int i = 1; s[cur++] == ','; ++i)
			args[i] = parse();
		--cur;
		return args;
	}
	int* parseArgs()
	{
		if (s[cur] == '(')
		{
			++cur;
			int* ans = parseArgs1();
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

vector<string> read_block(vector<string> prog, int it, int local[])
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

int exec(vector<string> prog, int local[]);

int exec_while(vector<string> prog, string expr, int local[])
{
	int res = 0;
	while (parser(expr, local).parse() && (res = exec(prog, local)) >= 0)
	{}
	if (res < 0)
		return res + 1; 
	return res;
}

int exec(vector<string> prog, int local[])
{
	for (int it = 0; it < prog.size(); ++it)
	{
		string s = prog[it];
		unspace(s);
		if (s[0] == 'g' && s[1] == 'c')
		{
			parser p(s.substr(2), local);
			char ch;
			cin >> ch;
			arr[p.parse()] = ch;
		}
		else if (s[0] == 'g')
		{
			parser p(s.substr(1), local);
			cin >> arr[p.parse()];
		}
		else if (s[0] == '[')
		{
			int p = find(s.begin(), s.end(), '=') - s.begin();
			string s1 = s.substr(1, p - 2);
			string s2 = s.substr(p + 1, s.size() - p - 1);
			parser p1(s1, local), p2(s2, local);
			int ind = p1.parse();
			int val = p2.parse();
			// cout << p << ' ' << s1 << ' ' << s2 << endl;
			arr[ind] = val;
		}
		else if (s[0] == '{')
		{
			int p = find(s.begin(), s.end(), '=') - s.begin();
			string s1 = s.substr(1, p - 2);
			string s2 = s.substr(p + 1, s.size() - p - 1);
			parser p1(s1, local), p2(s2, local);
			int ind = p1.parse();
			int val = p2.parse();
			// cout << p << ' ' << s1 << ' ' << s2 << endl;
			local[ind] = val;
		}
		else if (s[0] == 'p' && s[1] == 'c')
		{
			parser p(s.substr(2, s.size() - 1), local);
			cout << (char)p.parse();
		}
		else if (s[0] == 'p' && s[1] == 's')
		{
			cout << s.substr(3, s.size() - 3 - 1);
		}
		else if (s[0] == 'p')
		{
			parser p(s.substr(1, s.size() - 1), local);
			cout << p.parse();
		}
		else if (s[0] == 'r')
		{
			parser p(s.substr(1), local);
			return p.parse();
		}
		else if (s[0] == '/' && s[1] == '\\')
		{
			parser p(s.substr(2), local);
			return -p.parse();
		}
		else if (s[0] == 'w' && s[1] == 'h')
		{
			string expr = s.substr(2);
			vector<string> p = read_block(prog, it, local);
			int res = exec_while(p, expr, local);
			if (res < 0)
				return res;
			it += p.size() + 2;
		}
		else if (s[0] == 'd' && s[1] == 'f')
		{
			string name = s.substr(2);
			vector<string> p = read_block(prog, it, local);
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
			parser(expr, local).parse();
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

int func_call(string name, int* local)
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
	//string filename = argc > 1 ? argv[1] : "input.txt";
	//ifstream cin(filename);
	freopen("drill.in", "r", stdin);
	freopen("drill.out", "w", stdout);
	stringstream cin("df min\n{\n	{2} = {0}\n	wh {0} > {1}\n	{\n		{2} = {1}\n		/\\1\n	}\n	r {2}\n}\ng 0\ng 1\n[2] = 100\nwh [2] < 100 + [0]\n{\n	g [2]\n	[2] = [2] + 1\n}\n[2] = 100100\nwh [2] < 100100 + [1]\n{\n	g [2]\n	[2] = [2] + 1\n}\n[99] = -1000000000\n[100099] = -1000000000 \n[[0] + 100] = 1000000000\n[[1] + 100100] = 1000000000\n[-1] = 0\n[-2] = 0\n[-3] = 1000000000\nwh [-1] < [0] \n{\n	wh [[-2] + 100100] < [[-1] + 100]\n	{\n		[-2] = [-2] + 1\n	}\n	[-3] = min([-3], [[-2] + 100100] - [[-1] + 100])\n	[-3] = min([-3], [[-1] + 100] - [[-2] + 100099])\n	[-1] = [-1] + 1\n}\np [-3]\n");
	string s;
	vector<string> prog;
	while (getline(cin, s))
		prog.push_back(s);
	exec(prog, null);
	return 0;
}