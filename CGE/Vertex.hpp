#pragma once
#include "Math.hpp"
#include "Colour.hpp"

struct Vertex
{
	Vector4 Colour;
	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 Bitangent;
	Vector2 Texel;
};