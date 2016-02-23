//
// Created by Alexander Morozov on 20/02/16.
//

#include <set>
#include <iostream>
#include <fstream>
#include <ctime>
#include "builtins.hpp"
#include "function.hpp"

std::set<func_signature> built_in;

func_signature print("print", {"num", "flush"});
func_signature println("println", {"num"});
func_signature pc("pc", {"ord"});
func_signature gint("gint", {});
func_signature gc("gc", {});
func_signature cleck("clock", {});

void init_built_in()
{
    built_in.insert(print);
    built_in.insert(println);
    built_in.insert(pc);
    built_in.insert(gint);
    built_in.insert(gc);
    built_in.insert(cleck);
}

std::set<func_signature>& get_built_in()
{
    if (built_in.size() == 0)
        init_built_in();
    return built_in;
}

bool built_in_contains(string s)
{
    return s == "print" || s == "println" || s == "pc" || s == "gint" || s == "gc" || s == "clock";
}

VALUE call_built_in(string func, Vars local)
{
    if (func == print.name)
    {
        cout << local["num"];
        if (local.count("flush") && local["flush"])
            cout.flush();
        return VALUE();
    }
    if (func == println.name)
    {
        cout << local["num"] << std::endl;
        return VALUE();
    }
    if (func == pc.name)
    {
        cout << (char)(local["ord"].iv);
        return VALUE();
    }
    if (func == gint.name)
    {
        int x; cin >> x;
        return VALUE(x);
    }
    if (func == gc.name)
    {
        char x;
        cin.get(x);
        return VALUE(x);
    }
    if (func == cleck.name)
    {
        return VALUE((int)clock());
    }
    return VALUE();
}
