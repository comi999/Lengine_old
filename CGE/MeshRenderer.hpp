#pragma once
#include "Renderer.hpp"
#include "Rendering.hpp"
#include "Mesh.hpp"
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

	static void SampleColour( const Texture& a_Texture, Vector2 a_UV, Colour& o_Colour )
	{
		o_Colour = a_Texture.Sample( a_UV );
	}

	static void DrawTriangle( Vector2 a_P0, Vector2 a_P1, Vector2 a_P2, Vector2 a_V0, Vector2 a_V1, Vector2 a_V2, const Texture& a_Texture, float a_Intensity )
	{
		// Correct Y
		a_P0.y = -a_P0.y - 1.0f + ScreenBuffer::GetBufferHeight();
		a_P1.y = -a_P1.y - 1.0f + ScreenBuffer::GetBufferHeight();
		a_P2.y = -a_P2.y - 1.0f + ScreenBuffer::GetBufferHeight();

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

		// If points are colinear, reject.
		if ( a_P0.y == a_P2.y || a_P0.x == a_P2.x )
		{
			return;
		}

		// Align y.
		a_P0.y = static_cast< int >( a_P0.y );
		a_P1.y = static_cast< int >( a_P1.y );
		a_P2.y = static_cast< int >( a_P2.y );

		// Find M
		Vector2 M;

		if ( a_P0.y == a_P2.y )
		{
			M.y = a_P1.y;
			M.x = a_P0.x;
		}
		else
		{
			M.y = a_P1.y;
			M.x = a_P0.x - ( a_P0.y - a_P1.y ) * ( a_P0.x - a_P2.x ) / ( a_P0.y - a_P2.y );
		}

		float InvGrad = ( a_P0.x - a_P2.x ) / ( a_P0.y - a_P2.y );

		auto GetBarycentric = []( const Vector2& a_P0, const Vector2& a_P1, const Vector2& a_P2, const Vector2& a_P3 )
		{
			// ( y2 - y3 )( x - x3 ) + ( x3 - x2 )( y - y3 )
			// ( y2 - y3 )( x1 - x3 ) + ( x3 - x2 )( y1 - y3 )

			// ( y3 - y1 )( x - x3 ) + ( x1 - x3 )( y - y3 )
			// ( y2 - y3 )( x1 - x3 ) + ( x3 - x2 )( y1 - y3 )

			Vector3 Barycentric;
			Barycentric.x = ( a_P2.y - a_P3.y ) * ( a_P0.x - a_P3.x ) + ( a_P3.x - a_P2.x ) * ( a_P0.y - a_P3.y );
			Barycentric.x /= ( a_P2.y - a_P3.y ) * ( a_P1.x - a_P3.x ) + ( a_P3.x - a_P2.x ) * ( a_P1.y - a_P3.y );
			Barycentric.y = ( a_P3.y - a_P1.y ) * ( a_P0.x - a_P3.x ) + ( a_P1.x - a_P3.x ) * ( a_P0.y - a_P3.y );
			Barycentric.y /= ( a_P2.y - a_P3.y ) * ( a_P1.x - a_P3.x ) + ( a_P3.x - a_P2.x ) * ( a_P1.y - a_P3.y );
			Barycentric.z = 1.0f - Barycentric.x - Barycentric.y;

			return Barycentric;
		};
		auto GetCartesian   = []( const Vector3& a_BC, const Vector2& a_P1, const Vector2& a_P2, const Vector2& a_P3 )
		{
			return Vector2(
				a_BC.x * a_P1.x + a_BC.y * a_P2.x + a_BC.z * a_P3.x,
				a_BC.x * a_P1.y + a_BC.y * a_P2.y + a_BC.z * a_P3.y
			);
		};

		float tY = ( a_P1.y - a_P2.y ) / ( a_P0.y - a_P2.y );
		Vector2 TM(
			a_V2.x + ( a_V0.x - a_V2.x ) * tY,
			a_V2.y + ( a_V0.y - a_V2.y ) * tY );

		// Long side is on left
		if ( M.x <= a_P1.x )
		{
			float InvM;
			int XLeft, XRight, YMin, YMax;

			// Top
			if ( a_P0.y != a_P1.y )
			{
				InvM = ( a_P0.x - a_P1.x ) / ( a_P0.y - a_P1.y );
				YMin = Math::Max( a_P1.y, 0.0f );
				YMax = Math::Min( a_P0.y, -1.0f + ScreenBuffer::GetBufferHeight() );

				for ( int y = YMin; y < YMax; ++y )
				{
					XLeft = Math::Max( 0.0f, ( y - a_P0.y ) * InvGrad + a_P0.x );
					XRight = Math::Min( -1.0f + ScreenBuffer::GetBufferWidth(), ( y - a_P0.y ) * InvM + a_P0.x );
					//ScreenBuffer::SetColours( Vector< short, 2 >( XLeft, y ), a_Colour, XRight - XLeft );

					for ( int x = XLeft; x < XRight; ++x )
					{
						// Calculate barycentric coords
						auto BC = GetBarycentric( { x, y }, a_P0, M, a_P1 );
						auto CC = GetCartesian( BC, a_V0, TM, a_V1 );
						Colour Sampled = a_Texture.Sample( CC );
						Sampled.R *= a_Intensity;
						Sampled.G *= a_Intensity;
						Sampled.B *= a_Intensity;
						ScreenBuffer::SetColour( { x, y }, Sampled );
					}
				}
			}

			// Bottom
			if ( a_P1.y != a_P2.y )
			{
				InvM = ( a_P1.x - a_P2.x ) / ( a_P1.y - a_P2.y );
				YMin = Math::Max( a_P2.y, 0.0f );
				YMax = Math::Min( a_P1.y, -1.0f + ScreenBuffer::GetBufferHeight() );

				for ( int y = YMin; y < YMax; ++y )
				{
					XLeft = Math::Max( 0.0f, ( y - a_P2.y ) * InvGrad + a_P2.x );
					XRight = Math::Min( -1.0f + ScreenBuffer::GetBufferWidth(), ( y - a_P2.y ) * InvM + a_P2.x );
					//ScreenBuffer::SetColours( Vector< short, 2 >( XLeft, y ), a_Colour, XRight - XLeft );

					for ( int x = XLeft; x < XRight; ++x )
					{
						// Calculate barycentric coords
						auto BC = GetBarycentric( { x, y }, M, a_P1, a_P2 );
						auto CC = GetCartesian( BC, TM, a_V1, a_V2 );
						Colour Sampled = a_Texture.Sample( CC );
						Sampled.R *= a_Intensity;
						Sampled.G *= a_Intensity;
						Sampled.B *= a_Intensity;
						ScreenBuffer::SetColour( { x, y }, Sampled );
					}
				}
			}
		}

		// Long side is on right
		if ( M.x > a_P1.x )
		{
			float InvM;
			int XLeft, XRight, YMin, YMax;

			// Top
			if ( a_P0.y != a_P1.y )
			{
				InvM = ( a_P0.x - a_P1.x ) / ( a_P0.y - a_P1.y );
				YMin = Math::Max( a_P1.y, 0.0f );
				YMax = Math::Min( a_P0.y, -1.0f + ScreenBuffer::GetBufferHeight() );

				for ( int y = YMin; y < YMax; ++y )
				{
					XLeft = Math::Max( 0.0f, ( y - a_P0.y ) * InvM + a_P0.x );
					XRight = Math::Min( -1.0f + ScreenBuffer::GetBufferWidth(), ( y - a_P0.y ) * InvGrad + a_P0.x );
					//ScreenBuffer::SetColours( Vector< short, 2 >( XLeft, y ), a_Colour, XRight - XLeft );

					for ( int x = XLeft; x < XRight; ++x )
					{
						// Calculate barycentric coords
						auto BC = GetBarycentric( { x, y }, a_P0, a_P1, M );
						auto CC = GetCartesian( BC, a_V0, a_V1, TM );
						Colour Sampled = a_Texture.Sample( CC );
						Sampled.R *= a_Intensity;
						Sampled.G *= a_Intensity;
						Sampled.B *= a_Intensity;
						ScreenBuffer::SetColour( { x, y }, Sampled );
					}
				}
			}

			// Bottom
			if ( a_P1.y != a_P2.y )
			{
				InvM = ( a_P1.x - a_P2.x ) / ( a_P1.y - a_P2.y );
				YMin = Math::Max( a_P2.y, 0.0f );
				YMax = Math::Min( a_P1.y, -1.0f + ScreenBuffer::GetBufferHeight() );

				for ( int y = YMin; y < YMax; ++y )
				{
					XLeft = Math::Max( 0.0f, ( y - a_P2.y ) * InvM + a_P2.x );
					XRight = Math::Min( -1.0f + ScreenBuffer::GetBufferWidth(), ( y - a_P2.y ) * InvGrad + a_P2.x );
					//ScreenBuffer::SetColours( Vector< short, 2 >( XLeft, y ), a_Colour, XRight - XLeft );

					for ( int x = XLeft; x < XRight; ++x )
					{
						// Calculate barycentric coords
						auto BC = GetBarycentric( { x, y }, a_P1, M, a_P2 );
						auto CC = GetCartesian( BC, a_V1, TM, a_V2 );
						Colour Sampled = a_Texture.Sample( CC );
						Sampled.R *= a_Intensity;
						Sampled.G *= a_Intensity;
						Sampled.B *= a_Intensity;
						ScreenBuffer::SetColour( { x, y }, Sampled );
					}
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

		auto VertexCount = m_Mesh->GetVertexCount();
		auto TriangleCount = m_Mesh->GetVertexCount() / 3;
		auto Handle = Rendering::CreateRenderBuffer( VertexCount, sizeof( Vertex ) );
		auto NormalsHandle = Rendering::CreateRenderBuffer( TriangleCount, sizeof( Vector3 ) );
		auto Buffer = Rendering::GetRenderBuffer( Handle );
		auto Begin  = Buffer.Begin();
		auto NormalsBegin = Rendering::GetRenderBuffer( NormalsHandle ).Begin();
		Matrix4 PVM = Math::Multiply( Camera::GetMainCamera()->GetProjectionViewMatrix(), this->GetOwner().GetTransform()->GetGlobalMatrix() );

		for ( size_t i = 0; i < TriangleCount; ++i )
		{/*
			Vector3Int Triangle( i * 3, i * 3 + 1, i * 3 + 2 );
			Vector3 Pos[ 3 ] { m_Mesh->Vertices[ Triangle[ 0 ] ].Position, m_Mesh->Vertices[ Triangle[ 1 ] ].Position, m_Mesh->Vertices[ Triangle[ 2 ] ].Position };
			*/
			Vertex Vert[ 3 ];
			m_Mesh->GetTriangle( i, Vert );

			//auto TriangleNormal = objl::algorithm::GenTriNormal( Vert[ 0 ].Position, Vert[ 1 ].Position, Vert[ 2 ].Position );
			auto TriangleNormal = Math::Cross( Vert[ 0 ].Position - Vert[ 1 ].Position, Vert[ 0 ].Position - Vert[ 2 ].Position );
			TriangleNormal = Math::Normalize( Math::Multiply( this->GetOwner().GetTransform()->GetGlobalMatrix(), Vector4( TriangleNormal, 0.0f ) ) );
			NormalsBegin.Write( TriangleNormal, 0 );
			++NormalsBegin;

			for ( size_t j = 0; j < 3; ++j )
			{
				auto P = Math::Multiply( PVM, Vector4( Vert[ j ].Position, 1.0f ) );
				P /= P.w;
				P *= Vector4( ScreenBuffer::GetBufferWidth() * 0.5f, ScreenBuffer::GetBufferHeight() * 0.5f, 1.0f, 1.0f );
				P += Vector4( ScreenBuffer::GetBufferWidth() * 0.5f, ScreenBuffer::GetBufferHeight() * 0.5f, 0.0f, 0.0f );
				Begin.Write( P, 0 );
				Begin.Write( Vert[ j ].Texel, sizeof( P ) );
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
			//Colour c = *Begin.Read< Colour >( sizeof( Vector4 ) );
			auto v0 = Begin.Read< Vector4 >( 0 )->ToVector2();
			auto t0 = *Begin.Read< Vector2 >( sizeof( Vector4 ) );
			++Begin;
			auto v1 = Begin.Read< Vector4 >( 0 )->ToVector2();
			auto t1 = *Begin.Read< Vector2 >( sizeof( Vector4 ) );
			++Begin;
			auto v2 = Begin.Read< Vector4 >( 0 )->ToVector2();
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
				//Primitive::DrawTriangle( v0, v1, v2, Colour( 255 * Intensity, 255 * Intensity, 255 * Intensity, 255 ) );
				//Primitive::DrawTriangle( v0, v1, v2, c );
				DrawTriangle( v0, v1, v2, t0, t1, t2, *m_Texture, 1.0f );
			}
		}

		Rendering::DestroyRenderBuffer( Handle );
		Rendering::DestroyRenderBuffer( NormalsHandle );
	}

	void SetMesh( const Mesh* a_Mesh )
	{
		m_Mesh = a_Mesh;
	}

	void SetTexture( const Texture* a_Texture )
	{
		m_Texture = a_Texture;
	}

private:

	const Mesh* m_Mesh;
	const Texture* m_Texture;
};