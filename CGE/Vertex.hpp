#pragma once
#include "Math.hpp"
#include "Colour.hpp"

struct Vertex
{
	Colour Colour;
	Vector3 Position;
	Vector3 Normal;
	Vector2 Texel;
};