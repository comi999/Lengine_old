#include "Mesh.hpp"

Mesh Mesh::Cube = Mesh
{
	{ }, // Triangles
	{ }, // Vertices
	{ }, // Colours
	{    // Positions
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
	},
	{ },
	{ },
	static_cast< size_t >( -1 )
};

Mesh Mesh::Plane =
{

};