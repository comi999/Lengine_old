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

	void GetVertex( size_t a_Index, Vertex& o_Vertex ) const
	{
		uint32_t Index = m_Indices[ a_Index ];
		o_Vertex = 
		{
			m_Colours   [ Index ],
			m_Positions [ Index ],
			m_Normals   [ Index ],
			m_Tangents  [ Index ],
			m_Bitangents[ Index ],
			m_Texels    [ Index ]
		};
	}

	inline size_t GetVertexCount() const
	{
		return m_Indices.size();
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

//private:

	friend class ResourcePackager;
	friend class Serialization;

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << m_Indices << m_Colours << m_Positions << m_Normals << m_Tangents << m_Bitangents << m_Texels;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> m_Indices >> m_Colours >> m_Positions >> m_Normals >> m_Tangents >> m_Bitangents >> m_Texels;
	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{
		a_Sizer & m_Indices & m_Colours & m_Positions & m_Normals & m_Tangents & m_Bitangents & m_Texels;
	}

	std::vector< uint32_t > m_Indices;
	std::vector< Vector4  > m_Colours;
	std::vector< Vector3  > m_Positions;
	std::vector< Vector3  > m_Normals;
	std::vector< Vector3  > m_Tangents;
	std::vector< Vector3  > m_Bitangents;
	std::vector< Vector2  > m_Texels;
	size_t                  m_Outermost;
};