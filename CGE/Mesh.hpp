#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.hpp"

enum class EVertexComponent : uint8_t
{
	None,
	Colour,
	Position,
	Normal,
	Texel,
};

namespace Implementation
{
	constexpr auto GetLeading( unsigned int a_Value )
	{
		for ( int i = sizeof( unsigned int ) * 8 - 1; i >= 0; --i )
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
		for ( int i = sizeof( unsigned int ) * 8 - 1; i >= 0; --i )
		{
			if ( ( 1u << i ) & a_Value )
			{
				return a_Value & ~( 1u << i );
			}
		}
	
		return 0u;
	}
	
	struct VertexColour   { Colour  Colour;   };
	struct VertexPosition { Vector4 Position; };
	struct VertexNormal   { Vector3 Normal;   };
	struct VertexTexel    { Vector2 Texel;    };
	
	template < EVertexComponent Component > struct VertexComponentTypeImpl   { using Type = void          ; };
	template <> struct VertexComponentTypeImpl< EVertexComponent::Colour   > { using Type = VertexColour  ; };
	template <> struct VertexComponentTypeImpl< EVertexComponent::Position > { using Type = VertexPosition; };
	template <> struct VertexComponentTypeImpl< EVertexComponent::Normal   > { using Type = VertexNormal  ; };
	template <> struct VertexComponentTypeImpl< EVertexComponent::Texel    > { using Type = VertexTexel   ; };

	template < EVertexComponent Component >
	using VertexComponentType = typename VertexComponentTypeImpl< Component >::Type;
};

template < EVertexComponent... Flags >
struct Vertex : public Implementation::VertexComponentType< Flags >...
{ };

class Mesh
{
public:

	Mesh()
		: m_Outermost( -1 )
	{ }

	Mesh( 
		std::initializer_list< Vector< size_t, 3 > >&& a_Triangles,
		std::initializer_list< Vector< size_t, 4 > >&& a_Vertices,
		std::initializer_list< Colour              >&& a_Colours,
		std::initializer_list< Vector3             >&& a_Positions,
		std::initializer_list< Vector3             >&& a_Normals,
		std::initializer_list< Vector2             >&& a_Texels )
		: m_Triangles{ std::forward< std::initializer_list< Vector< size_t, 3 > > >( a_Triangles ) }
		, m_Vertices { std::forward< std::initializer_list< Vector< size_t, 4 > > >( a_Vertices  ) }
		, m_Colours  { std::forward< std::initializer_list< Colour              > >( a_Colours   ) }
		, m_Positions{ std::forward< std::initializer_list< Vector3             > >( a_Positions ) }
		, m_Normals  { std::forward< std::initializer_list< Vector3             > >( a_Normals   ) }
		, m_Texels   { std::forward< std::initializer_list< Vector2             > >( a_Texels    ) }
		, m_Outermost{ static_cast< size_t >( -1 ) }
	{
		FindOutermost();
	}

	template < EVertexComponent... Flags >
	void PopulateVertex( size_t a_Index, Vertex< Flags... >& a_Vertex ) const
	{
		auto& Vertex = m_Vertices[ a_Index ];

		if constexpr ( std::_Is_any_of_v< Implementation::VertexComponentType< EVertexComponent::Colour >, Implementation::VertexComponentType< Flags >... > )
		{
			a_Vertex.Colour = m_Colours[ Vertex[ 0 ] ];
		}

		if constexpr ( std::_Is_any_of_v< Implementation::VertexComponentType< EVertexComponent::Position >, Implementation::VertexComponentType< Flags >... > )
		{
			a_Vertex.Position = Vector4( m_Positions[ Vertex[ 1 ] ], 1.0f );
		}

		if constexpr ( std::_Is_any_of_v< Implementation::VertexComponentType< EVertexComponent::Normal >, Implementation::VertexComponentType< Flags >... > )
		{
			a_Vertex.Normal = m_Normals[ Vertex[ 2 ] ];
		}

		if constexpr ( std::_Is_any_of_v< Implementation::VertexComponentType< EVertexComponent::Texel >, Implementation::VertexComponentType< Flags >... > )
		{
			a_Vertex.Texel = m_Texels[ Vertex[ 3 ] ];
		}
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

public:

	static Mesh Cube;
	static Mesh Plane;
};