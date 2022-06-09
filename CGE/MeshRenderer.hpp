#pragma once
#include "Renderer.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Transform.hpp"
#include "Material.hpp"
#include "DrawCall.hpp"

// Remove later
#include "Camera.hpp"
#include "ScreenBuffer.hpp"
#include "Primitive.hpp"
#include "Light.hpp"
#include "File.hpp"
#include "Rect.hpp"

template < typename T >
class IMeshRenderer;

typedef IMeshRenderer< void > MeshRenderer;

template < typename T >
class IMeshRenderer : public IRenderer< IMeshRenderer< T > >
{
private:

	friend class RenderPipeline;	

public:

	void Draw()
	{
		if ( !Camera::GetMainCamera() || !m_Mesh )
		{
			return;
		}

		/*auto VertexCount = m_Mesh->GetVertexCount();
		auto TriangleCount = m_Mesh->GetVertexCount() / 3;
		auto Handle = Rendering::CreateRenderBuffer( VertexCount, sizeof( Vertex ) );
		auto NormalsHandle = Rendering::CreateRenderBuffer( TriangleCount, sizeof( Vector3 ) );
		auto Buffer = Rendering::GetRenderBuffer( Handle );
		auto Begin  = Buffer.Begin();
		auto NormalsBegin = Rendering::GetRenderBuffer( NormalsHandle ).Begin();
		Matrix4 PVM = Math::Multiply( Camera::GetMainCamera()->GetProjectionViewMatrix(), this->GetOwner().GetTransform()->GetGlobalMatrix() );

		for ( size_t i = 0; i < TriangleCount; ++i )
		{
			Vertex Vert[ 3 ];
			m_Mesh->GetTriangle( i, Vert );
			auto TriangleNormal = Math::Cross( Vert[ 0 ].Position - Vert[ 1 ].Position, Vert[ 0 ].Position - Vert[ 2 ].Position );
			TriangleNormal = Math::Normalize( Math::Multiply( this->GetOwner().GetTransform()->GetGlobalMatrix(), Vector4( TriangleNormal, 0.0f ) ) );
			NormalsBegin.Write( TriangleNormal, 0 );
			++NormalsBegin;

			for ( size_t j = 0; j < 3; ++j )
			{
				auto P = Math::Multiply( PVM, Vector4( Vert[ j ].Position, 1.0f ) );
				float w = P.w;
				P /= P.w;
				P.w = 1.0f / w;
				P *= Vector4( ScreenBuffer::GetBufferWidth() * 0.5f, ScreenBuffer::GetBufferHeight() * 0.5f, 1.0f, 1.0f );
				P += Vector4( ScreenBuffer::GetBufferWidth() * 0.5f, ScreenBuffer::GetBufferHeight() * 0.5f, 0.0f, 0.0f );
				Begin.Write( P, 0 );
				Begin.Write( Vert[ j ].Texel / w, sizeof( P ) );
				++Begin;
			}
		}

		Begin = Buffer.Begin();
		NormalsBegin = Rendering::GetRenderBuffer( NormalsHandle ).Begin();
		auto End = Buffer.End();

		auto AllLights = ECS::GetAllExactComponents< Light >();
		Light& L = *AllLights.raw();
		auto LightDirection = Math::Normalize( L.GetDirection() );

		for ( ; Begin != End; )
		{
			auto v0 = *Begin.Read< Vector4 >( 0 );
			auto t0 = *Begin.Read< Vector2 >( sizeof( Vector4 ) );
			++Begin;
			auto v1 = *Begin.Read< Vector4 >( 0 );
			auto t1 = *Begin.Read< Vector2 >( sizeof( Vector4 ) );
			++Begin;
			auto v2 = *Begin.Read< Vector4 >( 0 );
			auto t2 = *Begin.Read< Vector2 >( sizeof( Vector4 ) );
			++Begin;

			auto SurfaceNormal = Math::Normalize( Math::Cross( Vector3( v0 - v2 ), Vector3( v0 - v1 ) ) );
			auto TriangleNormal = *NormalsBegin.Read< Vector3 >( 0 );
			++NormalsBegin;
			auto Intensity = Math::Dot( TriangleNormal, -LightDirection );
			Intensity = Math::Clamp( Intensity, 0.5f, 1.0f );

			TriangleNormal += Vector3::One;
			TriangleNormal *= 0.5f;
			Colour c( 255 * TriangleNormal.x, 255 * TriangleNormal.y, 255 * TriangleNormal.z, 255 );

			c.R *= Intensity;
			c.G *= Intensity;
			c.B *= Intensity;

			if ( SurfaceNormal.z > 0.0f )
			{
				DrawTriangleEx( v0, v1, v2, t0, t1, t2, *m_Texture );
			}
		}

		Rendering::DestroyRenderBuffer( Handle );
		Rendering::DestroyRenderBuffer( NormalsHandle );*/
	}

	inline void SetMesh( const Mesh* a_Mesh )
	{
		m_Mesh = a_Mesh;
	}

	inline void SetMaterial( const Material* a_Material )
	{
		m_Material = a_Material;
	}

	inline void SetTexture( const Texture2D* a_Texture )
	{
		m_Texture = a_Texture;
	}

	inline void SetRenderMode( RenderMode a_RenderMode )
	{
		m_RenderMode = a_RenderMode;
	}

private:

	const Mesh*       m_Mesh;
	const Material*   m_Material;
	RenderMode        m_RenderMode;
	const Texture2D*  m_Texture;
};