#pragma once

#include <string>
#include <iostream>
#include <unordered_set>

#include "value.hpp"

using std::string;
using std::cin;
using std::cout;
using std::unordered_set;

void init_built_in();

bool built_in_contains(string s);

VALUE call_built_in(string name, VALUE* local);