//
// Created by Alexander Morozov on 20/02/16.
//

#include "builtins.hpp"

unordered_set<string> built_in;

void init_built_in()
{
    built_in.insert("print");
    built_in.insert("pi");
    built_in.insert("pc");

    built_in.insert("gint");
    built_in.insert("gi");
}

bool built_in_contains(string s)
{
    if (built_in.size() == 0)
        init_built_in();
    return built_in.count(s);
}

VALUE call_built_in(string name, VALUE* local)
{
    if (name == "print" || name == "pi")
    {
        cout << local[0];
        if (local[1].iv)
            cout.flush();
    }
    if (name == "pc")
    {
        cout << (char)(local[0].iv);
    }
    if (name == "gint" || name == "gi")
    {
        int x; cin >> x;
        return VALUE(x);
    }
    return VALUE();
}
