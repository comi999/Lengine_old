#include "CGE.h"
#include "Primitive.hpp"
#include "entt/entt.hpp"
#include "Input.hpp"

struct Cube
{
	Vector4 Corners[ 8 ]{
		Vector4( 0.5f,  0.5f, -0.5f, 1.0f ),
		Vector4( -0.5f,  0.5f, -0.5f, 1.0f ),
		Vector4( -0.5f, -0.5f, -0.5f, 1.0f ),
		Vector4( 0.5f, -0.5f, -0.5f, 1.0f ),
		Vector4( 0.5f,  0.5f,  0.5f, 1.0f ),
		Vector4( -0.5f,  0.5f,  0.5f, 1.0f ),
		Vector4( -0.5f, -0.5f,  0.5f, 1.0f ),
		Vector4( 0.5f, -0.5f,  0.5f, 1.0f ),
	};
};

struct Plane
{
	Vector4 Corners[ 36 ]{
		Vector4( -0.500f, 0.000f, -0.500f, 1.000f ), Vector4( -0.500f, 0.000f,  0.500f, 1.000f ),
		Vector4( -0.375f, 0.000f, -0.500f, 1.000f ), Vector4( -0.375f, 0.000f,  0.500f, 1.000f ),
		Vector4( -0.250f, 0.000f, -0.500f, 1.000f ), Vector4( -0.250f, 0.000f,  0.500f, 1.000f ),
		Vector4( -0.125f, 0.000f, -0.500f, 1.000f ), Vector4( -0.125f, 0.000f,  0.500f, 1.000f ),
		Vector4(  0.000f, 0.000f, -0.500f, 1.000f ), Vector4(  0.000f, 0.000f,  0.500f, 1.000f ),
		Vector4(  0.125f, 0.000f, -0.500f, 1.000f ), Vector4(  0.125f, 0.000f,  0.500f, 1.000f ),
		Vector4(  0.250f, 0.000f, -0.500f, 1.000f ), Vector4(  0.250f, 0.000f,  0.500f, 1.000f ),
		Vector4(  0.375f, 0.000f, -0.500f, 1.000f ), Vector4(  0.375f, 0.000f,  0.500f, 1.000f ),
		Vector4(  0.500f, 0.000f, -0.500f, 1.000f ), Vector4(  0.500f, 0.000f,  0.500f, 1.000f ),
		Vector4( -0.500f, 0.000f,-0.500f, 1.000f ), Vector4( 0.500f, 0.000f,-0.500f, 1.000f ),
		Vector4( -0.500f, 0.000f,-0.375f, 1.000f ), Vector4( 0.500f, 0.000f,-0.375f, 1.000f ),
		Vector4( -0.500f, 0.000f,-0.250f, 1.000f ), Vector4( 0.500f, 0.000f,-0.250f, 1.000f ),
		Vector4( -0.500f, 0.000f,-0.125f, 1.000f ), Vector4( 0.500f, 0.000f,-0.125f, 1.000f ),
		Vector4( -0.500f, 0.000f, 0.000f, 1.000f ), Vector4( 0.500f, 0.000f, 0.000f, 1.000f ),
		Vector4( -0.500f, 0.000f, 0.125f, 1.000f ), Vector4( 0.500f, 0.000f, 0.125f, 1.000f ),
		Vector4( -0.500f, 0.000f, 0.250f, 1.000f ), Vector4( 0.500f, 0.000f, 0.250f, 1.000f ),
		Vector4( -0.500f, 0.000f, 0.375f, 1.000f ), Vector4( 0.500f, 0.000f, 0.375f, 1.000f ),
		Vector4( -0.500f, 0.000f, 0.500f, 1.000f ), Vector4( 0.500f, 0.000f, 0.500f, 1.000f ),
	};
};

int main()
{
	CGE::Initialize( "Some window!", { 128, 128 }, { 1, 1 } );
	Input::Initialize();
	Cube cube;
	Plane plane;

	Vector3 CubePosition = Vector3( 0.0f, 0.5f, 0.0f );
	Vector3 CubeRotation = Vector3::Zero;
	Vector3 CubeScale = Vector3::One * 1.0f;

	Vector3 PlanePosition = Vector3( 0, 0, 0 );
	Vector3 PlaneRotation = Vector3::Zero;
	Vector3 PlaneScale = Vector3::One * 5.0f;

	Vector3 CameraPosition = Vector3( 0.0f, 0.0f, -3.0f );
	Vector3 CameraRotation = Vector3( Math::Radians( -0.0f ), 0.0f, 0.0f );

	auto ProjectionMatrix = Matrix4::CreateProjection( Math::Radians( 90.0f ), 1.0f, 1.0f, 100.0f );
	auto ViewMatrix = Matrix4();
	auto VPMatrix = Matrix4();
	Vector3 forward;
	Vector3 right;
	Vector3 up;

	auto drawCube = [&]()
	{
		Matrix4 CubeMatrix = Matrix4::CreateTransform( CubePosition, CubeRotation, CubeScale );
		auto MVP = Math::Multiply( CubeMatrix, VPMatrix );
		Vector4 Verts[ 8 ];

		for ( int i = 0; i < 8; ++i )
		{
			Verts[ i ] = Math::Multiply( cube.Corners[ i ], MVP );
			Verts[ i ] /= Verts[ i ].w;
			Verts[ i ].z = Math::Max( Verts[ i ].z, 0.0f );
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
	};

	auto drawPlane = [&]()
	{
		Matrix4 PlaneMatrix = Matrix4::CreateTransform( PlanePosition, PlaneRotation, PlaneScale );
		auto MVP = Math::Multiply( PlaneMatrix, VPMatrix );
		Vector4 Verts[ 36 ];

		for ( int i = 0; i < 36; ++i )
		{
			Verts[ i ] = Math::Multiply( plane.Corners[ i ], MVP );
			Verts[ i ] /= Verts[ i ].w;
			Verts[ i ].z = Math::Max( Verts[ i ].z, 0.0f );
			Verts[ i ] *= Vector4( 64, 64, 1, 1 );
			Verts[ i ] += Vector4( 64, 64, 0, 0 );
		}

		for ( int i = 0; i < 35; i += 2 )
		{
			Primitive::DrawLine( Verts[ i ].ToVector2(), Verts[ i + 1 ].ToVector2(), Colour::RED );
		}

		/*for ( int i = 19; i < 27; ++i )
		{
			Primitive::DrawLine( Verts[ i ].ToVector2(), Verts[ i + 9 ].ToVector2(), Colour::RED );
		}*/
	};

	bool Running = true;
	float movement = 0.01f;
	float sensitivity = 0.0000001f;

	while ( Running )
	{
		ViewMatrix = Matrix4::CreateView( CameraPosition, CameraRotation );
		VPMatrix = Math::Multiply( ViewMatrix, ProjectionMatrix );
		forward = ViewMatrix.c2.ToVector3();
		right = ViewMatrix.c0.ToVector3();
		up = ViewMatrix.c1.ToVector3();

		ScreenBuffer::SetBuffer( Colour::BLACK );
		drawPlane();
		drawCube();


		// Left
		if ( Input::IsKeyDown( KeyCode::A ) )
		{
			//CameraPosition.x -= 0.1f;
			CameraPosition -= movement * right;
		}

		// Right
		if ( Input::IsKeyDown( KeyCode::D ) )
		{
			//CameraPosition.x += 0.1f;
			CameraPosition += movement * right;
		}

		// Forward
		if ( Input::IsKeyDown( KeyCode::W ) )
		{
			//CameraPosition.z += 0.1f;
			CameraPosition += movement * forward;
		}

		// Back
		if ( Input::IsKeyDown( KeyCode::S ) )
		{
			//CameraPosition.z -= 0.1f;
			CameraPosition -= movement * forward;
		}

		// Up
		if ( Input::IsKeyDown( KeyCode::E ) )
		{
			//CameraPosition.y += 0.1f;
			CameraPosition += movement * up;
		}

		// Down
		if ( Input::IsKeyDown( KeyCode::Q ) )
		{
			//CameraPosition.y -= 0.1f;
			CameraPosition -= movement * up;
		}

		if ( Input::IsKeyDown( KeyCode::M ) )
		{
			Running = false;
		}

		//CubeRotation.y += 0.01f;
		//CubeRotation.x += 0.01f;

		//CameraRotation.y += 0.001f;
		Vector2 MouseDelta = Input::GetMouseDelta();
		CameraRotation.y += Math::Radians( MouseDelta.x * 1.0f );
		CameraRotation.x += Math::Radians( MouseDelta.y * 1.0f );

		Input::Tick();
		ConsoleWindow::WriteBuffer();
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

	
	
	Input::Deinitialize();
	return 0;
}