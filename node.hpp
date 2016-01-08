#pragma once

#include "optype.hpp"
#include "value.hpp"

struct nd
{
	nd* left;
	nd* right;

	op_type op;
	VALUE value;
	VALUE exec(VALUE* local);
	nd(op_type op, nd* left, nd* right);
	nd(op_type op, VALUE value);
};
