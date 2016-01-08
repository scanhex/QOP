#include <iostream>

#include "value.hpp"

std::ostream& operator <<(std::ostream& out, VALUE& x)
{
	if (x.t == VALUE::INT)
		return out << x.iv;
	return out;
}