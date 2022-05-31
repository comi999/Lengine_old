#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.hpp"
#include "Vertex.hpp"

class Mesh
{
public:

	Mesh()
		: m_Outermost( size_t( -1 ) )
	{ }

	void GetTriangle( size_t a_Index, Vertex( &o_Triangle )[ 3 ] ) const
	{
		const Vector< size_t, 4 >* Vertex = &m_Vertices[ a_Index * 3 ];

		o_Triangle[ 0 ] =
		{
			Colour::WHITE, //m_Colours  [ Vertex[ 0 ][ 0 ] ],
			m_Positions[ Vertex[ 0 ][ 1 ] ],
			m_Normals  [ Vertex[ 0 ][ 2 ] ],
			m_Texels.size() ? m_Texels[ Vertex[ 0 ][ 3 ] ] : Vector2::Zero
		};

		o_Triangle[ 1 ] =
		{
			Colour::WHITE, //m_Colours  [ Vertex[ 1 ][ 0 ] ],
			m_Positions[ Vertex[ 1 ][ 1 ] ],
			m_Normals  [ Vertex[ 1 ][ 2 ] ],
			m_Texels.size() ? m_Texels[ Vertex[ 1 ][ 3 ] ] : Vector2::Zero
		};

		o_Triangle[ 2 ] =
		{
			Colour::WHITE, //m_Colours  [ Vertex[ 2 ][ 0 ] ],
			m_Positions[ Vertex[ 2 ][ 1 ] ],
			m_Normals  [ Vertex[ 2 ][ 2 ] ],
			m_Texels.size() ? m_Texels   [ Vertex[ 2 ][ 3 ] ] : Vector2::Zero
		};
	}

	void GetVertex( size_t a_Index, Vertex& o_Vertex ) const
	{
		auto& Vertex = m_Vertices[ a_Index ];
		o_Vertex = 
		{
			m_Colours  [ Vertex[ 0 ] ],
			m_Positions[ Vertex[ 1 ] ],
			m_Normals  [ Vertex[ 2 ] ],
			m_Texels   [ Vertex[ 3 ] ]
		};
	}

	inline size_t GetVertexCount() const
	{
		return m_Vertices.size();
	}

	inline float GetRadius() const
	{
		if ( m_Outermost == size_t( -1 ) )
		{
			// Find outmost point.
			float RadiusSqrd = 0.0f;

			for ( size_t i = 0; i < m_Positions.size(); ++i )
			{
				float CurrentRadiusSqrd = Math::LengthSqrd( m_Positions[ i ] );

				if ( CurrentRadiusSqrd < RadiusSqrd )
				{
					const_cast< Mesh* >( this )->m_Outermost = i;
					RadiusSqrd = CurrentRadiusSqrd;
				}
			}

			return RadiusSqrd;
		}

		return Math::LengthSqrd( m_Positions[ m_Outermost ] );
	}

private:

	friend class ResourcePackager;
	friend class Serialization;

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << m_Vertices << m_Colours << m_Positions << m_Normals << m_Texels;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> m_Vertices >> m_Colours >> m_Positions >> m_Normals >> m_Texels;
	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{
		a_Sizer & m_Vertices & m_Colours & m_Positions & m_Normals & m_Texels;
	}

	std::vector< Vector< size_t, 4 > > m_Vertices;
	std::vector< Colour  >             m_Colours;
	std::vector< Vector3 >             m_Positions;
	std::vector< Vector3 >             m_Normals;
	std::vector< Vector2 >             m_Texels;
	size_t                             m_Outermost;
};