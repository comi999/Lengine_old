#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.h"



class Mesh
{
public:

	Mesh()
		: m_Radius( -1.0f )
	{ }

	float GetRadius() const
	{
		if ( m_Radius < 0.0f )
		{
			const_cast< Mesh* >( this )->CalculateRadius();
		}

		return m_Radius;
	}

private:

	void CalculateRadius()
	{
		m_Radius = 0.0f;

		for ( const auto& Position : m_Positions )
		{
			float LengthSqrd = Math::LengthSqrd( Position );

			if ( LengthSqrd > m_Radius )
			{
				m_Radius = LengthSqrd;
			}
		}

		m_Radius = Math::Sqrt( m_Radius );
	}

	friend class Graphics;

	std::vector< Vector3Int > m_Triangles;
	std::vector< Vector4Int > m_Vertices;
	std::vector< Colour  >    m_Colours;
	std::vector< Vector3 >    m_Positions;
	std::vector< Vector3 >    m_Normals;
	std::vector< Vector2 >    m_Texels;
	float                     m_Radius;
};