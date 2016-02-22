#pragma once

#include <string>
#include <vector>
#include "node.hpp"


using std::string;
using std::vector;

class func_signature
{
public:
    string name;
    vector<string> args;
    nd* body;
    func_signature(string name, vector<string> args)
        : name(name), args(args), body(NULL)
    {}

    bool operator < (const func_signature& b) const
    {
        if (name != b.name)
            return name < b.name;
        return args.size() < b.args.size();
    }

    bool operator == (const func_signature& b) const
    {
        return name == b.name;
    }
};

