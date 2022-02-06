#pragma once
#include "Math.h"

struct Rect
{
	tVector2< int > Origin;
	tVector2< int > Size;

	inline int GetLeft() const
	{
		return Origin.i;
	}

	inline int GetRight() const
	{
		return Origin.i + Size.i - 1;
	}

	inline int GetBottom() const
	{
		return Origin.j + Size.j - 1;
	}

	inline int GetTop() const
	{
		return Origin.j;
	}
};