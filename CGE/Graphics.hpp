#pragma once
#include "Shader.hpp"
#include "Mesh.hpp"

struct Vertex
{
	Colour  Colour;
	Vector3 Position;
	Vector3 Normal;
	Vector2 Texel;
};

struct Triangle
{
	Vertex Vertices[ 3 ];
};

// broad phase filtering: remove OBJECTS that will definitely not show up on screen by using encompassing regions and frustum planes
// vertex shader transform vertices into clip space
// everything is now in clip space
// clipping
// move from clip space to ndc space / could be perspective correct/affine/flat
// ndc to screen space
// fragment shader when rasterizing triangle on screen

class Graphics
{
private:

	inline static Triangle GetTriangleFromMesh( size_t a_Index, const Mesh& a_Mesh )
	{
		Triangle Result;
		const Vector3Int& Tri = a_Mesh.m_Triangles[ a_Index ];

		/*if ( InterpolateColours )
		{
			Result.Vertices[ 0 ].Colour = &a_Mesh.m_Colours[ a_Mesh.m_Vertices[ Tri.x ].x ];
			Result.Vertices[ 1 ].Colour = &a_Mesh.m_Colours[ a_Mesh.m_Vertices[ Tri.y ].x ];
			Result.Vertices[ 2 ].Colour = &a_Mesh.m_Colours[ a_Mesh.m_Vertices[ Tri.z ].x ];
		}

		if ( InterpolatePositions )
		{
			Result.Vertices[ 0 ].Position = &a_Mesh.m_Positions[ a_Mesh.m_Vertices[ Tri.x ].y ];
			Result.Vertices[ 1 ].Position = &a_Mesh.m_Positions[ a_Mesh.m_Vertices[ Tri.y ].y ];
			Result.Vertices[ 2 ].Position = &a_Mesh.m_Positions[ a_Mesh.m_Vertices[ Tri.z ].y ];
		}

		if ( InterpolateNormals )
		{
			Result.Vertices[ 0 ].Normal = &a_Mesh.m_Normals[ a_Mesh.m_Vertices[ Tri.x ].z ];
			Result.Vertices[ 1 ].Normal = &a_Mesh.m_Normals[ a_Mesh.m_Vertices[ Tri.y ].z ];
			Result.Vertices[ 2 ].Normal = &a_Mesh.m_Normals[ a_Mesh.m_Vertices[ Tri.z ].z ];
		}

		if ( InterpolateTexels )
		{
			Result.Vertices[ 0 ].Texel = &a_Mesh.m_Texels[ a_Mesh.m_Vertices[ Tri.x ].w ];
			Result.Vertices[ 1 ].Texel = &a_Mesh.m_Texels[ a_Mesh.m_Vertices[ Tri.y ].w ];
			Result.Vertices[ 2 ].Texel = &a_Mesh.m_Texels[ a_Mesh.m_Vertices[ Tri.z ].w ];
		}*/

		return Result;
	}

	static void Interpolate(  )
	{
		
	}

public:

	static bool InterpolateColours;
	static bool InterpolateNormals;
	static bool InterpolatePositions;
	static bool InterpolateTexels;
	static bool BackFaceCullingEnabled;
};