#include <string>
#include <vector> 
#include <unordered_map>

#include "defines.hpp"
#include "node.hpp"
#include "builtins.hpp"


using std::string;
using std::vector;
using std::unordered_map;
using std::pair;
using std::cin;
using std::cout;

const int SZ = 500000;
const int LCSZ = 32;

VALUE* arr = new VALUE[2 * SZ] + SZ;

unordered_map<string, VALUE> vars;

vector<pair<string, VALUE>> changed_vars;


VALUE func_call(string name, VALUE* local)
{
	if (built_in_contains(name))
	{
		return call_built_in(name, local);
	}
	else
	{
		auto x = ((nd*)vars[name].data)->exec(local);
		return x;
	}
}

void set_var(string name, VALUE val)
{
	auto old = vars.find(name);
	if (old != vars.end())
		changed_vars.push_back(*old);
	vars[name] = val;
}

VALUE nd::exec(VALUE* local)
{
	if (op == VAR)
	{
		return value;
	}
	else
	{
		if (op == NOT)
			return !left->exec(local);
		if (op == NEG)
			return -left->exec(local);
		if (op == PLUS)
		{
			return left->exec(local) + right->exec(local);
		}
		if (op == MINUS)
		{
			return left->exec(local) - right->exec(local);
		}
		if (op == MULT)
		{
			return left->exec(local) * right->exec(local);
		}
		if (op == DIV)
		{
			return left->exec(local) / right->exec(local);
		}
		if (op == MOD)
		{
			return left->exec(local) % right->exec(local);
		}
		if (op == OR)
		{
			return left->exec(local) | right->exec(local);
		}
		if (op == AND)
		{
			return left->exec(local) & right->exec(local);
		}
		if (op == XOR)
		{
			return left->exec(local) ^ right->exec(local);
		}
		if (op == GAND)
		{
			VALUE x = left->exec(local);
			if ((bool)x == false)
				return false;
			return right->exec(local);
		}
		if (op == GOR)
		{
			VALUE x = left->exec(local);
			if ((bool)x == true)
				return true;
			return right->exec(local);
		}
		if (op == LESS)
		{
			return left->exec(local) < right->exec(local);
		}
		if (op == GREATER)
		{
			return left->exec(local) > right->exec(local);
		}
		if (op == EQUALS)
		{
			return left->exec(local) == right->exec(local);
		}
		if (op == BRACES)
		{
			return local[left->exec(local).iv];
		}
		if (op == BRACKETS)
		{
			return arr[left->exec(local).iv];
		}
		if (op == FUNC_CALL)
		{
			assert(right->value.t == VALUE::TUPLE);
			nd** tup = (nd**)(right->value.data);
			VALUE* args = new VALUE[tup[0]->value.iv];
			for (int i = 0; i < tup[0]->value.iv; ++i)
				args[i] = tup[i + 1]->exec(local);
			return func_call(*(string*)(left->value.data), args);
		}
		if (op == FUNC_DEF)
		{
			assert(left->value.t == VALUE::STRING);
			vars[*(string*)left->value.data] = VALUE(VALUE::FUNCTION, right);
		}
		if (op == NEWLINE)
		{
			VALUE res = left->exec(local);
			VALUE res1 = right->exec(local);
			if (res1.t != VALUE::VOID)
				return res1;
			else
				return res;
		}
		if (op == WHILE)
		{
			VALUE last;
			while ((bool)(left->exec(local)))
				last = right->exec(local);
			return last;
		}
		if (op == IF)
		{
			if ((bool)(left->exec(local)))
				return right->exec(local);
		}
		if (op == ASSIGN)
		{
			if (left->op == BRACKETS)
			{
				int ind = left->left->exec(local).iv;
				VALUE val = right->exec(local);
				arr[ind] = val;
			}
			else if (left->op == BRACES)
			{
				int ind = left->left->exec(local).iv;
				VALUE val = right->exec(local);
				local[ind] = val;
			}
		}
		assert(false || "Unknown operation");
	}
}

nd::nd(op_type op, nd* left, nd* right)
	: op(op), left(left), right(right), value(VALUE())
{}
nd::nd(op_type op, VALUE value)
	: op(op), value(value)
{}
