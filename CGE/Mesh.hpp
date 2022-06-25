#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.hpp"
#include "Vertex.hpp"
#include "Resource.hpp"

class Mesh : public Resource
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

	inline uint32_t GetVertexCount() const
	{
		return m_Positions.size();
	}

	inline uint32_t GetIndexCount() const
	{
		return m_Indices.size();
	}

	inline uint32_t GetOutermost() const
	{
		if ( m_Outermost == uint32_t( -1 ) )
		{
			float RadiusSqrd = 0.0f;

			for ( uint32_t i = 0; i < m_Positions.size(); ++i )
			{
				float CurrentRadiusSqrd = Math::LengthSqrd( m_Positions[ i ] );

				if ( CurrentRadiusSqrd > RadiusSqrd )
				{
					const_cast< Mesh* >( this )->m_Outermost = i;
					RadiusSqrd = CurrentRadiusSqrd;
				}
			}
		}

		return m_Outermost;
	}

	inline float GetRadius() const
	{
		return Math::Length( m_Positions[ GetOutermost() ] );
	}

	inline const uint32_t* GetIndices() const
	{
		return m_Indices.data();
	}

	inline const Vector4* GetColours() const
	{
		return m_Colours.data();
	}

	inline const Vector3* GetPositions() const
	{
		return m_Positions.data();
	}

	inline const Vector3* GetNormals() const
	{
		return m_Normals.data();
	}

	inline const Vector3* GetTangents() const
	{
		return m_Tangents.data();
	}

	inline const Vector3* GetBitangents() const
	{
		return m_Bitangents.data();
	}

	inline const Vector2* GetTexels() const
	{
		return m_Texels.data();
	}

	inline bool HasColours() const
	{
		return !m_Colours.empty();
	}

	inline bool HasPositions() const
	{
		return !m_Positions.empty();
	}

	inline bool HasNormals() const
	{
		return !m_Normals.empty();
	}

	inline bool HasTangents() const
	{
		return !m_Tangents.empty();
	}

	inline bool HasBitangents() const
	{
		return !m_Bitangents.empty();
	}

	inline bool HasTexels() const
	{
		return !m_Texels.empty();
	}

//private:

	friend class ResourcePackager;
	friend class Serialization;

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << *static_cast< const Resource* >( this );
		a_Serializer << m_Indices << m_Colours << m_Positions << m_Normals << m_Tangents << m_Bitangents << m_Texels;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> *static_cast< Resource* >( this );
		a_Deserializer >> m_Indices >> m_Colours >> m_Positions >> m_Normals >> m_Tangents >> m_Bitangents >> m_Texels;
	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{
		a_Sizer & *static_cast< const Resource* >( this );
		a_Sizer & m_Indices & m_Colours & m_Positions & m_Normals & m_Tangents & m_Bitangents & m_Texels;
	}

	std::vector< uint32_t > m_Indices;
	std::vector< Vector4  > m_Colours;
	std::vector< Vector3  > m_Positions;
	std::vector< Vector3  > m_Normals;
	std::vector< Vector3  > m_Tangents;
	std::vector< Vector3  > m_Bitangents;
	std::vector< Vector2  > m_Texels;
	uint32_t                m_Outermost;
};