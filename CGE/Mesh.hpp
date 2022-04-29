#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.hpp"
#include "Vertex.hpp"
#include "Material.hpp"

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

	void PopulateVertex( size_t a_Index, Vertex& a_Vertex ) const
	{
		auto& Vertex      = m_Vertices [ a_Index ];
		a_Vertex.Colour   = m_Colours  [ Vertex[ 0 ] ];
		a_Vertex.Position = m_Positions[ Vertex[ 1 ] ];
		a_Vertex.Normal   = m_Normals  [ Vertex[ 2 ] ];
		a_Vertex.Texel    = m_Texels   [ Vertex[ 3 ] ];
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

struct Mesh
{
	// Default Constructor
	Mesh()
	{

	}

	// Variable Set Constructor
	Mesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices)
	{
		Vertices = _Vertices;
		Indices = _Indices;
	}

	// Mesh Name
	std::string MeshName;
	// Vertex List
	std::vector<Vertex> Vertices;
	// Index List
	std::vector<unsigned int> Indices;

	// Material
	Material MeshMaterial;
};