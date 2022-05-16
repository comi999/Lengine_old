#pragma once
#include "Renderer.hpp"
#include "Rendering.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "ScreenBuffer.hpp"
#include "Primitive.hpp"
#include "Light.hpp"
#include "File.hpp"

template < typename T >
class IMeshRenderer;

typedef IMeshRenderer< void > MeshRenderer;

template < typename T >
class IMeshRenderer : public IRenderer< IMeshRenderer< T > >
{
public:

	void Draw()
	{
		if ( !Camera::GetMainCamera() || !m_Mesh )
		{
			return;
		}

		//auto VertexCount = m_Mesh->Indices.size();
		//auto TriangleCount = m_Mesh->Indices.size() / 3;
		////auto VertexCount = m_Mesh->m_Triangles.size() * 3;
		//auto Handle = Rendering::CreateRenderBuffer( VertexCount, sizeof( Vertex ) );
		//auto NormalsHandle = Rendering::CreateRenderBuffer( TriangleCount, sizeof( Vector3 ) );
		//auto Buffer = Rendering::GetRenderBuffer( Handle );
		//auto Begin  = Buffer.Begin();
		//auto NormalsBegin = Rendering::GetRenderBuffer( NormalsHandle ).Begin();
		//Matrix4 PVM = Math::Multiply( Camera::GetMainCamera()->GetProjectionViewMatrix(), this->GetOwner().GetTransform()->GetGlobalMatrix() );

		//for ( size_t i = 0; i < TriangleCount; ++i )
		//{
		//	//auto& Triangle = m_Mesh->m_Triangles[ i ];
		//	Vector3Int Triangle( i * 3, i * 3 + 1, i * 3 + 2 );
		//	Vector3 Pos[ 3 ] { m_Mesh->Vertices[ Triangle[ 0 ] ].Position, m_Mesh->Vertices[ Triangle[ 1 ] ].Position, m_Mesh->Vertices[ Triangle[ 2 ] ].Position };
		//	Vertex Vert;

		//	auto TriangleNormal = objl::algorithm::GenTriNormal( Pos[ 0 ], Pos[ 1 ], Pos[ 2 ] );
		//	TriangleNormal = Math::Normalize( Math::Multiply( this->GetOwner().GetTransform()->GetGlobalMatrix(), Vector4( TriangleNormal, 0.0f ) ) );
		//	NormalsBegin.Write( TriangleNormal, 0 );
		//	++NormalsBegin;

		//	for ( size_t j = 0; j < 3; ++j )
		//	{
		//		auto P = Math::Multiply( PVM, Vector4( Pos[ j ], 1.0f ) );
		//		P /= P.w;
		//		P *= Vector4( ScreenBuffer::GetBufferWidth() * 0.5f, ScreenBuffer::GetBufferHeight() * 0.5f, 1.0f, 1.0f );
		//		P += Vector4( ScreenBuffer::GetBufferWidth() * 0.5f, ScreenBuffer::GetBufferHeight() * 0.5f, 0.0f, 0.0f );
		//		Begin.Write( P, 0 );
		//		++Begin;
		//	}
		//}

		//Begin = Buffer.Begin();
		//NormalsBegin = Rendering::GetRenderBuffer( NormalsHandle ).Begin();
		//auto End = Buffer.End();

		//auto AllLights = ECS::GetAll< Light >();
		//Light& L = *AllLights.raw();
		//auto LightDirection = Math::Normalize( L.GetDirection() );

		//for ( ; Begin != End; )
		//{
		//	//Colour c = *Begin.Read< Colour >( sizeof( Vector4 ) );
		//	auto v0 = Begin.Read< Vector4 >( 0 )->ToVector2();
		//	++Begin;
		//	auto v1 = Begin.Read< Vector4 >( 0 )->ToVector2();
		//	++Begin;
		//	auto v2 = Begin.Read< Vector4 >( 0 )->ToVector2();
		//	++Begin;

		//	auto SurfaceNormal = Math::Normalize( Math::Cross( Vector3( v0 - v2 ), Vector3( v0 - v1 ) ) );
		//	auto TriangleNormal = *NormalsBegin.Read< Vector3 >( 0 );
		//	++NormalsBegin;
		//	auto Intensity = Math::Dot( TriangleNormal, -LightDirection );
		//	Intensity = Math::Max( 0.0f, Intensity );

		//	TriangleNormal += Vector3::One;
		//	TriangleNormal *= 0.5f;
		//	Colour c( 255 * TriangleNormal.x, 255 * TriangleNormal.y, 255 * TriangleNormal.z, 255 );

		//	c.R *= Intensity;
		//	c.G *= Intensity;
		//	c.B *= Intensity;

		//	if ( SurfaceNormal.z > 0.0f )
		//	{
		//		//Primitive::DrawTriangle( v0, v1, v2, Colour( 255 * Intensity, 255 * Intensity, 255 * Intensity, 255 ) );
		//		Primitive::DrawTriangle( v0, v1, v2, c );
		//	}
		//}

		//Rendering::DestroyRenderBuffer( Handle );
		//Rendering::DestroyRenderBuffer( NormalsHandle );
	}

	void SetMesh( const Mesh* a_Mesh )
	{
		m_Mesh = a_Mesh;
	}

private:

	const Mesh* m_Mesh;
};