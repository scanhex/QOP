#include <string>
#include <vector> 
#include <unordered_map>

#include "defines.hpp"
#include "node.hpp"
#include "builtins.hpp"
#include "function.hpp"


using std::string;
using std::vector;
using std::unordered_map;
using std::pair;
using std::cin;
using std::cerr;
using std::cout;

const int SZ = 500000;

Vars vars;

VALUE *arr = new VALUE[2 * SZ] + SZ;

void runtime(string msg)
{
    cerr << "Runtime exception has occured. Message: " << msg << std::endl;
}

inline void add_built_in()
{
    static bool built_in_added;
    if (!built_in_added)
    {
        auto bi = get_built_in();
        for (auto x : bi)
            vars[x.name] = VALUE(VALUE::FUNCTION, new func_signature(x));
        built_in_added = true;
    }
}

VALUE func_call(string name, Vars local)
{
    add_built_in();
    if (built_in_contains(name))
    {
        return call_built_in(name, local);
    }
    else
    {
        auto sign = (func_signature *) vars[name].data;
        auto x = sign->body->exec(local);
        return x;
    }
}

bool func_exists(string name)
{
    add_built_in();
    return vars.count(name) && vars[name].t == VALUE::FUNCTION;
}

VALUE nd::exec(Vars &local)
{
    if (op == NUMBER || op == VAL)
        return value;
    if (op == VAR)
    {
        string name = *(string *) value.data;
        if (local.count(name))
            return local[name];
        else if (vars.count(name))
            return vars[name];
        else
            runtime("No such variable: " + name);
    }
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
        if ((bool) x == false)
            return false;
        return right->exec(local);
    }
    if (op == GOR)
    {
        VALUE x = left->exec(local);
        if ((bool) x == true)
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
    if (op == LEQ)
    {
        return left->exec(local) <= right->exec(local);
    }
    if (op == GREQ)
    {
        return left->exec(local) >= right->exec(local);
    }
    if (op == EQUALS)
    {
        return left->exec(local) == right->exec(local);
    }
    if (op == NEQUALS)
    {
        return left->exec(local) != right->exec(local);
    }
//    if (op == BRACES)
//    {
//        return local[left->exec(local).iv];
//    }
    if (op == BRACKETS)
    {
        return arr[left->exec(local).iv];
    }
    if (op == FUNC_CALL)
    {
        assert(right->value.t == VALUE::TUPLE);
        string name = *(string *) left->value.data;
        assert(func_exists(name));
        auto sign = *(func_signature *) vars[name].data;
        VALUE *tup = (VALUE *) (right->value.data);
        int cnt = ((nd *) tup[0].data)->value.iv;
        Vars args;
        for (int i = 0; i < cnt; ++i)
            args[sign.args[i]] = ((nd *) tup[i + 1].data)->exec(local);
        return func_call(name, args);
    }
    if (op == FUNC_DEF)
    {
        assert(value.t == VALUE::FUNCTION);
        auto fs = *(func_signature *) value.data;
        vars[fs.name] = value;
        return VALUE();
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
        while ((bool) (left->exec(local)))
            last = right->exec(local);
        return last;
    }
    if (op == IF)
    {
        if (right->op == ELSE)
        {
            if ((bool) left->exec(local))
                return right->left->exec(local);
            else
                return right->right->exec(local);
        }
        else if ((bool) (left->exec(local)))
            return right->exec(local);
        return VALUE();
    }
    if (op == ASSIGN)
    {
        if (left->op == VAR)
        {
            string name = *(string *) left->value.data;
            VALUE val = right->exec(local);
            if (name[0] == '@')
            {
                name.erase(name.begin());
                vars[name] = val;
            }
            else
                local[name] = val;
            return val;
        }
        else if (left->op == BRACKETS)
        {
            int ind = left->left->exec(local).iv;
            VALUE val = right->exec(local);
            arr[ind] = val;
            return val;
        }
//        else if (left->op == BRACES)
//        {
//            int ind = left->left->exec(local).iv;
//            VALUE val = right->exec(local);
//            local[ind] = val;
//        }
        return VALUE();
    }
    if (op == NIL)
        return VALUE();
    assert(false && "Unknown operation");
    return VALUE();
}

VALUE nd::exec()
{
    return exec(vars);
}

nd::nd(op_type op, nd *left, nd *right)
        : op(op), left(left), right(right), value(VALUE()) { }

nd::nd(op_type op, VALUE value)
        : op(op), value(value), left(NULL), right(NULL) { }
