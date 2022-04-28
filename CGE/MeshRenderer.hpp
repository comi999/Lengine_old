#pragma once
#include "Renderer.hpp"
#include "Rendering.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "ScreenBuffer.hpp"
#include "Primitive.hpp"
#include "Light.hpp"

template < typename T >
class IMeshRenderer;

typedef IMeshRenderer< void > MeshRenderer;

template < typename T >
class IMeshRenderer : public IRenderer< IMeshRenderer< T > >
{
public:

	void Draw()
	{
		if ( !Camera::GetMainCamera() )
		{
			return;
		}

		auto VertexCount = m_Mesh->m_Triangles.size() * 3;
		auto Handle = Rendering::CreateRenderBuffer( VertexCount, sizeof( Vertex< EVertexComponent::Position, EVertexComponent::Colour > ) );
		auto NormalsHandle = Rendering::CreateRenderBuffer( m_Mesh->m_Triangles.size(), sizeof( Vector3 ) );
		auto Buffer = Rendering::GetRenderBuffer( Handle );
		auto Begin  = Buffer.Begin();
		auto NormalsBegin = Rendering::GetRenderBuffer( NormalsHandle ).Begin();
		Matrix4 PVM = Math::Multiply( Camera::GetMainCamera()->GetProjectionViewMatrix(), this->GetOwner().GetTransform()->GetGlobalMatrix() );

		for ( size_t i = 0; i < m_Mesh->m_Triangles.size(); ++i )
		{
			auto& Triangle = m_Mesh->m_Triangles[ i ];
			Vertex< EVertexComponent::Position, EVertexComponent::Colour > Vert;

			auto TriangleNormal = m_Mesh->m_Normals[ m_Mesh->m_Vertices[ Triangle[ 0 ] ][ 2 ] ];
			TriangleNormal = Math::Normalize( Math::Multiply( this->GetOwner().GetTransform()->GetGlobalMatrix(), Vector4( TriangleNormal, 0.0f ) ) );
			NormalsBegin.Write( TriangleNormal, 0 );
			++NormalsBegin;

			for ( size_t j = 0; j < 3; ++j )
			{
				auto Index = Triangle[ j ];
				m_Mesh->PopulateVertex( Index, Vert );
				auto P = Math::Multiply( PVM, Vert.Position );
				P /= P.w;
				P *= Vector4( ScreenBuffer::GetBufferWidth() * 0.5f, ScreenBuffer::GetBufferHeight() * 0.5f, 1.0f, 1.0f );
				P += Vector4( ScreenBuffer::GetBufferWidth() * 0.5f, ScreenBuffer::GetBufferHeight() * 0.5f, 0.0f, 0.0f );
				Begin.Write( P, 0 );
				Begin.Write( Vert.Colour, sizeof( Vector4 ) );
				++Begin;
			}
		}

		Begin = Buffer.Begin();
		NormalsBegin = Rendering::GetRenderBuffer( NormalsHandle ).Begin();
		auto End = Buffer.End();

		auto AllLights = ECS::GetAll< Light >();
		Light& L = *AllLights.raw();
		auto LightDirection = Math::Normalize( L.GetDirection() );

		for ( ; Begin != End; )
		{
			Colour c = *Begin.Read< Colour >( sizeof( Vector4 ) );
			auto v0 = Begin.Read< Vector4 >( 0 )->ToVector2();
			++Begin;
			auto v1 = Begin.Read< Vector4 >( 0 )->ToVector2();
			++Begin;
			auto v2 = Begin.Read< Vector4 >( 0 )->ToVector2();
			++Begin;

			auto SurfaceNormal = Math::Normalize( Math::Cross( Vector3( v0 - v2 ), Vector3( v0 - v1 ) ) );
			auto TriangleNormal = *NormalsBegin.Read< Vector3 >( 0 );
			++NormalsBegin;
			auto Intensity = Math::Dot( TriangleNormal, -LightDirection );
			Intensity = Math::Max( 0.0f, Intensity );

			c.R *= Intensity;
			c.G *= Intensity;
			c.B *= Intensity;

			if ( SurfaceNormal.z < 0.0f )
			{
				Primitive::DrawTriangle( v0, v1, v2, Colour( 255 * Intensity, 255 * Intensity, 255 * Intensity, 255 ) );
			}
		}

		Rendering::DestroyRenderBuffer( Handle );
		Rendering::DestroyRenderBuffer( NormalsHandle );
	}

	void SetMesh( const Mesh* a_Mesh )
	{
		m_Mesh = a_Mesh;
	}

private:

	const Mesh* m_Mesh;
};