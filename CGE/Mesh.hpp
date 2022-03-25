#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.h"

enum class VertexIncludeFlags
{
	Colour   = 0x1,
	Position = 0x2,
	Normal   = 0x4,
	Texel    = 0x8
};

struct VertexReference
{
private:

	friend struct TriangleReference;
	friend class Mesh;

	VertexReference( const Colour& a_Colour, const Vector3& a_Position, const Vector3& a_Normal, const Vector2& a_Texel )
		: VertexColour( a_Colour )
		, VertexPosition( a_Position )
		, VertexNormal( a_Normal )
		, VertexTexel( a_Texel )
	{ }

public:

	const Colour&  VertexColour;
	const Vector3& VertexPosition;
	const Vector3& VertexNormal;
	const Vector2& VertexTexel;
};

struct TriangleReference
{
private:

	friend class Mesh;

	TriangleReference( const VertexReference& a_P0, const VertexReference& a_P1, const VertexReference& a_P2 )
		: Vertices{ a_P0, a_P1, a_P2 }
	{ }

public:

	union
	{
		VertexReference Vertices[ 3 ];
		struct { VertexReference p0, p1, p2; };
	};
};

class Mesh
{
public:

	Mesh()
		: m_Outermost( -1 )
	{ }

	VertexReference GetVertex( size_t a_Index ) const
	{
		auto& Vertex = m_Vertices[ a_Index ];
		return VertexReference(
			m_Colours  [ Vertex[ 0 ] ],
			m_Positions[ Vertex[ 1 ] ],
			m_Normals  [ Vertex[ 2 ] ],
			m_Texels   [ Vertex[ 3 ] ]
		);
	}

	TriangleReference GetTriangle( size_t a_Index ) const
	{
		auto& Triangle = m_Triangles[ a_Index ];
		return TriangleReference(
			{
				GetVertex( Triangle[ 0 ] ),
				GetVertex( Triangle[ 1 ] ),
				GetVertex( Triangle[ 2 ] )
			} );
	}

	const Vector3& GetOutermostPosition() const
	{
		if ( m_Outermost == static_cast< size_t >( -1 ) )
		{
			const_cast< Mesh* >( this )->FindOutermost();
		}

		return m_Positions[ m_Outermost ];
	}

	inline float GetRadius() const
	{
		return Math::Length( GetOutermostPosition() );
	}

private:

	void FindOutermost()
	{
		float GreatestDistanceSqrd = 0.0f;

		for ( size_t i = 0, size = m_Positions.size(); i < size; ++i )
		{
			float DistanceSqrd = Math::LengthSqrd( m_Positions[ i ] );

			if ( DistanceSqrd > GreatestDistanceSqrd )
			{
				GreatestDistanceSqrd = DistanceSqrd;
				m_Outermost = i;
			}
		}
	}

	friend class Graphics;

	std::vector< Vector< size_t, 3 > > m_Triangles;
	std::vector< Vector< size_t, 4 > > m_Vertices;
	std::vector< Colour  >             m_Colours;
	std::vector< Vector3 >             m_Positions;
	std::vector< Vector3 >             m_Normals;
	std::vector< Vector2 >             m_Texels;
	size_t                             m_Outermost;

	static Mesh Cube;
	static Mesh Plane;
};