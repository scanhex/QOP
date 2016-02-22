#pragma once

#include <string>
#include <iostream>
#include <set>

#include "value.hpp"
#include "defines.hpp"
#include "function.hpp"

using std::string;
using std::cin;
using std::cout;
using std::set;

std::set<func_signature>& get_built_in();

void init_built_in();

bool built_in_contains(string s);

VALUE call_built_in(string name, Vars local);