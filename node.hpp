#pragma once

#include "optype.hpp"
#include "value.hpp"
#include "defines.hpp"

class nd
{
public:
	nd* left;
	nd* right;

	op_type op;
	VALUE value;
	VALUE exec(Vars& local);
	VALUE exec();
	nd(op_type op, nd* left, nd* right);
	nd(op_type op, VALUE value);
};
