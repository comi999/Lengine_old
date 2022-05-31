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

	DrawCall GenerateDrawCall()
	{
		const Matrix4* Model = &Component::GetOwner().GetTransform()->GetGlobalMatrix();
		return { m_Mesh, m_Material, Model, m_RenderMode };
	}

	static void DrawTriangleEx( Vector4 a_P0, Vector4 a_P1, Vector4 a_P2, Vector2 a_V0, Vector2 a_V1, Vector2 a_V2, const Texture& a_Texture )
	{
		// Correct Y
		a_P0.y = -a_P0.y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();
		a_P1.y = -a_P1.y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();
		a_P2.y = -a_P2.y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();

		// Sort corners.
		if ( a_P0.y < a_P1.y )
		{
			std::swap( a_P0, a_P1 );
			std::swap( a_V0, a_V1 );
		}

		if ( a_P0.y < a_P2.y )
		{
			std::swap( a_P0, a_P2 );
			std::swap( a_V0, a_V2 );
		}

		if ( a_P1.y < a_P2.y )
		{
			std::swap( a_P1, a_P2 );
			std::swap( a_V1, a_V2 );
		}

		if ( a_P0.y == a_P2.y )
		{
			return;
		}

		// Align y.
		a_P0.y = static_cast< int >( a_P0.y );
		a_P1.y = static_cast< int >( a_P1.y );
		a_P2.y = static_cast< int >( a_P2.y );

		// Find M
		float L = ( a_P1.y - a_P2.y ) / ( a_P0.y - a_P2.y );
		Vector4 PM = Math::Lerp( L, a_P2, a_P0 );
		PM.y = static_cast< int >( PM.y );
		Vector2 TM = Math::Lerp( L, a_V2, a_V0 );

		// Swap P1 and M if M is not on right.
		if ( PM.x < a_P1.x )
		{
			std::swap( PM, a_P1 );
			std::swap( TM, a_V1 );
		}

		float SpanX, SpanY, y;
		Vector4 PStepL, PStepR, PBegin, PStep, PL, PR;
		Vector2 VStepL, VStepR, VBegin, VStep, VL, VR;

		if ( a_P2.y != a_P1.y )
		{
			SpanY = 1.0f / ( a_P1.y - a_P2.y );

			PStepL = a_P1 - a_P2; PStepL *= SpanY;
			PStepR = PM - a_P2; PStepR *= SpanY;
			PL = a_P2;
			PR = a_P2;

			VStepL = a_V1 - a_V2; VStepL *= SpanY;
			VStepR = TM - a_V2; VStepR *= SpanY;
			VL = a_V2;
			VR = a_V2;

			y = a_P2.y;

			for ( ; y < a_P1.y; ++y )
			{
				PL += PStepL;
				PR += PStepR;
				VL += VStepL;
				VR += VStepR;

				SpanX = 1.0f / ( PR.x - PL.x );
				PBegin = PL;
				PStep = ( PR - PL ) * SpanX;
				VBegin = VL;
				VStep = ( VR - VL ) * SpanX;

				for ( ; PBegin.x < PR.x; PBegin += PStep, VBegin += VStep )
				{
					ScreenBuffer::SetColour( { PBegin.x, y }, a_Texture.Sample( VBegin / PBegin.w ) );
				}
			}
		}
		else
		{
			y = a_P1.y;
			PL = a_P1, PR = PM;
			VL = a_V1, VR = TM;
		}

		if ( a_P1.y != a_P0.y )
		{
			SpanY = 1.0f / ( a_P0.y - a_P1.y );

			PStepL = a_P0 - a_P1; PStepL *= SpanY;
			PStepR = a_P0 - PM; PStepR *= SpanY;

			VStepL = a_V0 - a_V1; VStepL *= SpanY;
			VStepR = a_V0 - TM; VStepR *= SpanY;

			for ( ; y < a_P0.y; ++y )
			{
				PL += PStepL;
				PR += PStepR;
				VL += VStepL;
				VR += VStepR;

				SpanX = 1.0f / ( PR.x - PL.x );
				PBegin = PL;
				PStep = ( PR - PL ) * SpanX;
				VBegin = VL;
				VStep = ( VR - VL ) * SpanX;

				for ( ; PBegin.x < PR.x; PBegin += PStep, VBegin += VStep )
				{
					ScreenBuffer::SetColour( { PBegin.x, y }, a_Texture.Sample( VBegin / PBegin.w ) );
				}
			}
		}
	}

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

	inline void SetTexture( const Texture* a_Texture )
	{
		m_Texture = a_Texture;
	}

	inline void SetRenderMode( RenderMode a_RenderMode )
	{
		m_RenderMode = a_RenderMode;
	}

private:

	const Mesh*     m_Mesh;
	const Material* m_Material;
	RenderMode      m_RenderMode;
	const Texture*  m_Texture;
};