#pragma once
#include "Math.hpp"

struct Rect
{
	Vector2Int Origin;
	Vector2Int Size;

	inline int GetLeft() const
	{
		return Origin.x;
	}

	inline int GetRight() const
	{
		return Origin.x + Size.x - 1;
	}

	inline int GetBottom() const
	{
		return Origin.y + Size.y - 1;
	}

	inline int GetTop() const
	{
		return Origin.y;
	}
};