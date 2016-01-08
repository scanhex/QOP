#include <algorithm>
#include <string>
#include <vector>
#include "main.cpp"
//#include "Parser.h"	
using namespace std;

struct parser
{
	string s;
	int cur;
	parser(string s)
		: s(s + "$"), cur(0)
	{}
	int parseInt()
	{
		if (s[cur] == '(')
		{
			++cur;
			int ans = parseSum();
			++cur;
			return ans;
		}
		if (s[cur] == '[')
		{
			++cur;
			int ind = parseInt();
			if (ind < -500000 || ind > 500000) return (rand() << 16)  + rand();
			++cur;
			return arr[ind];
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
};