#include "CGE.h"
#include "Primitive.hpp"
#include "entt/entt.hpp"
#include "Input.hpp"

int main()
{
	CGE::Initialize( "Some window!", { 128, 128 }, { 1, 1 } );
	Input::Initialize();

	struct Cube
	{
		Vector4 Corners[ 8 ] {
			Vector4(  0.5f,  0.5f, -0.5f, 1.0f ),
			Vector4( -0.5f,  0.5f, -0.5f, 1.0f ),
			Vector4( -0.5f, -0.5f, -0.5f, 1.0f ),
			Vector4(  0.5f, -0.5f, -0.5f, 1.0f ),
			Vector4(  0.5f,  0.5f,  0.5f, 1.0f ),
			Vector4( -0.5f,  0.5f,  0.5f, 1.0f ),
			Vector4( -0.5f, -0.5f,  0.5f, 1.0f ),
			Vector4(  0.5f, -0.5f,  0.5f, 1.0f ),
		};
	};

	Cube cube;
	Vector3 CubePosition = Vector3::Zero;
	Vector3 CubeRotation = Vector3::Zero;
	Vector3 CubeScale = Vector3::One * 1.f;
	
	Vector3 CameraPosition = Vector3( 0.0f, 0.0f, -3.0f );
	Vector3 CameraRotation = Vector3( Math::Radians( -0.0f ), 0.0f, 0.0f );

	
	auto ProjectionMatrix = Math::Transpose( Matrix4::CreateProjection( Math::Radians( 75.0f ), 1.0f, 1.0f, 100.0f ) );

	auto drawCube = [&]()
	{
		Input::Tick();
		auto ViewMatrix = Matrix4::CreateView( CameraPosition, CameraRotation );
		Matrix4 CubeMatrix = Matrix4::CreateTransform( CubePosition, CubeRotation, CubeScale );
		auto PVMMatrix = Math::Multiply( CubeMatrix, Math::Multiply( ViewMatrix, ProjectionMatrix ) );

		Vector4 Verts[ 8 ];

		ScreenBuffer::SetBuffer( Colour::BLACK );

		for ( int i = 0; i < 8; ++i )
		{
			Verts[ i ] = Math::Multiply( cube.Corners[ i ], PVMMatrix );
			Verts[ i ] /= Verts[ i ].w;
			Verts[ i ] *= Vector4( 64, 64, 1, 1 );
			Verts[ i ] += Vector4( 64, 64, 0, 0 );
		}

		Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 1 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 2 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 3 ].ToVector2(), Verts[ 0 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 4 ].ToVector2(), Verts[ 5 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 5 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 6 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 7 ].ToVector2(), Verts[ 4 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 4 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 1 ].ToVector2(), Verts[ 5 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 2 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour::WHITE );
		Primitive::DrawLine( Verts[ 3 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour::WHITE );

		ConsoleWindow::WriteBuffer();
	};

	float X = 0.0f;

	

	while ( true )
	{
		drawCube();
		//CameraPosition.y = Math::Sin( X += 0.005f );
		//CameraPosition.y = 1.0f;
		//CameraPosition.x = Math::Sin( X += 0.005f );
		//CameraPosition.z = Math::Cos( X );

		// A
		//if ( IsKeyDown( 0x41 ) )
		//{
		//	CameraPosition.x += 0.1f;
		//}

		//// D
		//if ( IsKeyDown( 0x44 ) )
		//{
		//	CameraPosition.x -= 0.1f;
		//}

		//// W
		//if ( IsKeyDown( 0x57 ) )
		//{
		//	CameraPosition.z += 0.1f;
		//}

		//// S
		//if ( IsKeyDown( 0x53 ) )
		//{
		//	CameraPosition.z -= 0.1f;
		//}

		//// E
		//if ( IsKeyDown( 0x45 ) )
		//{
		//	CameraPosition.y += 0.1f;
		//}

		//// Q
		//if ( IsKeyDown( 0x51 ) )
		//{
		//	CameraPosition.y -= 0.1f;
		//}

		CubeRotation.y += 0.01f;
		CubeRotation.x += 0.01f;
	}
	
	/*for ( int x = 0; x < 256; x += 1 )
	{
		Sleep( 25 );
		for ( int y = 0; y < 128; ++y )
		{
			for ( int z = 0; z < 128; ++z )
			{
				ScreenBuffer::SetPixel( { short(y), short(z) }, ScreenBuffer::GetPixelColourMap().ConvertColour( Colour( x, y * 2, z * 2 ) ) );
			}
		}
		ConsoleWindow::WriteBuffer();

		if ( x == 252 )
		{
			x = 0;
		}
	}*/

	/*Rect rect1 = { { 1, 1 }, { 45, 20 } };
	Rect rect2 = { { 5, 15 }, { 45, 100 } };
	ScreenBuffer::BlendingEnabled = true;
	float f = 0.0f;
	while ( true )
	{
		f += 0.1f;
		ScreenBuffer::SetBuffer( Colour::WHITE );
		ScreenBuffer::SetRect( rect1, Colour::YELLOW );
		ScreenBuffer::SetRect( rect2, Colour( Colour::DARK_BLUE, 32 ) );
		Primitive::DrawLine( { 64, 64 }, { sin( f ) * 90 + 64, cos( f ) * 90 + 64 }, Colour( Colour::DARK_RED, 128 ) );
		ConsoleWindow::WriteBuffer();
	}*/

	
	
	
	return 0;
}