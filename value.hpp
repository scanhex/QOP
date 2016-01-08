#pragma once

#include <cassert>

struct VALUE
{
	enum type
	{
		INT, STRING, DOUBLE, TUPLE, FUNCTION, VOID
	};
	type t;
	union
	{
		void* data;
		int iv;
		double dv;
		char cv;
	};

	VALUE(type t, void* data)
		: t(t), data(data)
	{}
	VALUE(int iv)
		: t(INT), iv(iv)
	{}
	VALUE(double dv)
		: t(DOUBLE), dv(dv)
	{}
	VALUE()
		: t(VOID), data(0)
	{}

	VALUE operator !()
	{
		if (t == INT)
			return VALUE((int)!iv);
		if (t == DOUBLE)
			return VALUE((double)!dv);
		return VALUE();
	}
	VALUE operator -()
	{
		if (t == INT)
			return VALUE((int)-iv);
		if (t == DOUBLE)
			return VALUE((double)-dv);
		return VALUE();
	}
	VALUE operator +(VALUE other)
	{
		if (t == INT)
			return VALUE(iv + other.iv);
		return VALUE();
	}
	VALUE operator -(VALUE other)
	{
		if (t == INT)
			return VALUE(iv - other.iv);
		return VALUE();
	}
	VALUE operator *(VALUE other)
	{
		if (t == INT)
			return VALUE(iv * other.iv);
		return VALUE();
	}
	VALUE operator /(VALUE other)
	{
		if (t == INT)
			return VALUE(iv / other.iv);
		return VALUE();
	}
	VALUE operator %(VALUE other)
	{
		if (t == INT)
			return VALUE(iv % other.iv);
		return VALUE();
	}
	VALUE operator &(VALUE other)
	{
		if (t == INT)
			return VALUE(iv & other.iv);
		return VALUE();
	}
	VALUE operator |(VALUE other)
	{
		if (t == INT)
			return VALUE(iv | other.iv);
		return VALUE();
	}
	VALUE operator ^(VALUE other)
	{
		if (t == INT)
			return VALUE(iv ^ other.iv);
		return VALUE();
	}
	VALUE operator <(VALUE other)
	{
		if (t == INT)
			return VALUE(iv < other.iv);
		return VALUE();
	}
	VALUE operator >(VALUE other)
	{
		if (t == INT)
			return VALUE(iv > other.iv);
		return VALUE();
	}
	VALUE operator ==(VALUE other)
	{
		if (t == INT)
			return VALUE(iv == other.iv);
		return VALUE();
	}
	explicit operator bool()
	{
		if (t == INT)
			return (bool)iv;
		if (t == DOUBLE)
			return (bool)dv;
		assert(false && "Type is not int or double");
	}
};

std::ostream& operator <<(std::ostream& out, VALUE& x);