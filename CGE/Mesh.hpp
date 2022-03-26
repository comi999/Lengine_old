#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.h"

enum class VertexIncludeFlags
{
	None     = 0x0,
	Colour   = 0x1,
	Position = 0x2,
	Normal   = 0x4,
	Texel    = 0x8,
};

//struct VertexColour   { Colour  Colour;   };
//struct VertexPosition { Vector3 Position; };
//struct VertexNormal   { Vector3 Normal;   };
//struct VertexTexel    { Vector2 Texel;    };
//
//template < VertexIncludeFlags Flag > struct VertexTypeImpl        { using Type = void;           };
//template <> struct VertexTypeImpl< VertexIncludeFlags::Colour   > { using Type = VertexColour;   };
//template <> struct VertexTypeImpl< VertexIncludeFlags::Position > { using Type = VertexPosition; };
//template <> struct VertexTypeImpl< VertexIncludeFlags::Normal   > { using Type = VertexNormal;   };
//template <> struct VertexTypeImpl< VertexIncludeFlags::Texel    > { using Type = VertexTexel;    };
//
//template < VertexIncludeFlags Flags, VertexIncludeFlags Leading = Flags & VertexIncludeFlags::Colour >
//struct VertexInterface
//{
//	
//};

template < VertexIncludeFlags Flags >
struct Vertex : public Vertex< Flags >> 1 >
{
private:


};

struct Triangle
{
};

class Mesh
{
public:

	Mesh()
		: m_Outermost( -1 )
	{ }

	/*Vertex GetVertex( size_t a_Index ) const
	{
		return Vertex();
	}

	Triangle GetTriangle( size_t a_Index ) const
	{
		return Triangle();
	}*/

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