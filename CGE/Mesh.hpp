#pragma once
#include <vector>
#include "Math.hpp"
#include "Colour.h"



class Mesh
{
public:

	

private:

	std::vector< Vector3Int > m_Triangles;
	std::vector< Vector4Int > m_Vertices;
	std::vector< Colour  >    m_Colours;
	std::vector< Vector3 >    m_Positions;
	std::vector< Vector3 >    m_Normals;
	std::vector< Vector2 >    m_Texels;

private:

	friend class Graphics;
};