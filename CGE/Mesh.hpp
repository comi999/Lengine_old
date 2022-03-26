#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.h"

constexpr auto GetLeading( unsigned int a_Value )
{
	for ( size_t i = sizeof( unsigned int ) * 8 - 1; i >= 0; --i )
	{
		if ( ( 1u << i ) & a_Value )
		{
			return 1u << i;
		}
	}

	return 0u;
}

constexpr auto RemoveLeading( unsigned int a_Value )
{
	for ( size_t i = sizeof( unsigned int ) * 8 - 1; i >= 0; --i )
	{
		if ( ( 1u << i ) & a_Value )
		{
			return a_Value & ~( 1u << i );
		}
	}

	return 0u;
}

enum class VertexIncludeFlags
{
	None     = 0x0,
	Colour   = 0x1,
	Position = 0x2,
	Normal   = 0x4,
	Texel    = 0x8
};

constexpr VertexIncludeFlags operator |( VertexIncludeFlags a_Left, VertexIncludeFlags a_Right )
{
	return static_cast< VertexIncludeFlags >( static_cast< unsigned int >( a_Left ) | static_cast< unsigned int >( a_Right ) );
}

struct VertexColour   { Colour  Colour;   };
struct VertexPosition { Vector3 Position; };
struct VertexNormal   { Vector3 Normal;   };
struct VertexTexel    { Vector2 Texel;    };

template < VertexIncludeFlags Flag > struct VertexTypeImpl        { using Type = std::tuple<>;                  };
template <> struct VertexTypeImpl< VertexIncludeFlags::Colour   > { using Type = std::tuple < VertexColour   >; };
template <> struct VertexTypeImpl< VertexIncludeFlags::Position > { using Type = std::tuple < VertexPosition >; };
template <> struct VertexTypeImpl< VertexIncludeFlags::Normal   > { using Type = std::tuple < VertexNormal   >; };
template <> struct VertexTypeImpl< VertexIncludeFlags::Texel    > { using Type = std::tuple < VertexTexel    >; };

template < unsigned int Flags >
struct VertexInterfaceImpl
{
	static constexpr VertexIncludeFlags Leading = static_cast< VertexIncludeFlags >( GetLeading( Flags ) );
	static constexpr unsigned int Rest = RemoveLeading( Flags );
	using Type = typename VertexTypeImpl< Leading >::Type;
	using Tuple = std::conditional_t< ( Rest > 0u ), typename std::_Tuple_cat1< Type, typename VertexInterfaceImpl< Rest >::Tuple >::type, Type >;
};

template < VertexIncludeFlags flags >
using VertexInterface = VertexInterfaceImpl< static_cast< unsigned int >( flags ) >;

template < typename Tuple >
struct RecursiveInheritorImpl : protected RecursiveInheritorImpl< typename Tuple::_Mybase >, public Tuple::_This_type
{ };

template < typename... Bases >
using RecursiveInheritor = RecursiveInheritorImpl< std::tuple< Bases... > >;

template <>
struct RecursiveInheritorImpl< std::tuple<> >
{ };

void foo()
{
	RecursiveInheritor< VertexColour, VertexNormal > a;
}

struct Vertex
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