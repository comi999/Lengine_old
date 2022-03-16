#pragma once
#include "Renderer.hpp"
#include "Mesh.hpp"

template < typename T >
class IMeshRenderer;

typedef IMeshRenderer< void > MeshRenderer;

template < typename T >
class IMeshRenderer : public IRenderer< IMeshRenderer< T > >
{
public:

	void Draw()
	{

	}

	void SetMesh( const Mesh& a_Mesh )
	{
		m_Mesh = &a_Mesh;
	}

private:

	Mesh* m_Mesh;
};