#include "CGE.h"
#include "Primitive.hpp"
#include "entt/entt.hpp"
#include "Input.hpp"
#include "Component.hpp"

struct Cube
{
	Vector4 Corners[ 8 ]
	{
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
	Vector4 Corners[ 36 ]
	{
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

struct Axes
{
	Vector4 Corners[ 4 ]
	{
		Vector4( Vector3::Zero, 1.0f ),
		Vector4( Vector3::Right, 1.0f ),
		Vector4( Vector3::Up, 1.0f ),
		Vector4( Vector3::Forward, 1.0f )
	};
};

#include "Transform.hpp"
#include "GameObject.hpp"
void foo()
{

}
int main()
{
	/*auto e0 = Vector3( Math::Radians( 10.0f ), Math::Radians( 20.0f ), Math::Radians( 15.0f ) );
	auto q0 = Quaternion::ToQuaternion( e0, RotationOrder::XYZ );
	auto e1 = Quaternion::ToEulerAngles( q0, RotationOrder::XYZ );
	e1 = Vector3( Math::Degrees( e1.x ), Math::Degrees( e1.y ), Math::Degrees( e1.z ) );
	auto m0 = Matrix4::CreateRotation( q0 );
	auto q1 = Quaternion::ToQuaternion( m0 );
	auto e2 = Quaternion::ToEulerAngles( q1, RotationOrder::XYZ );
	e2 = Vector3( Math::Degrees( e2.x ), Math::Degrees( e2.y ), Math::Degrees( e2.z ) );*/

	auto v0 = Vector3::Right;
	//auto q0 = Quaternion::ToQuaternion( Vector3::Up, Math::Radians( 90.0f ) );
	auto q0 = Quaternion::ToQuaternion( Vector3( 0.0f, Math::Radians( .0f ), 0.0f ), RotationOrder::XYZ );
	auto m0 = Quaternion::ToMatrix3( q0 );
	auto q1 = Quaternion::ToQuaternion( m0 );
	auto m1 = Quaternion::ToMatrix3( q1 );
	
	auto r0 = Math::Multiply( m1, v0 );
	
	Vector3 pos;
	Vector3 sca;
	Quaternion qua;
	auto tra = Matrix4::CreateTransform( 
		Vector3( 10, 20, 30 ), 
		Quaternion::ToQuaternion( Vector3( Math::Radians( 10.0f ), Math::Radians( 20.0f ), Math::Radians( 15.0f ) ), RotationOrder::XYZ ), 
		Vector3( 2, 3, 4 ) );

	Matrix4::Decompose( tra, pos, qua, sca );
	foo();
	//CGE::Initialize( "Some window!", { 128, 128 }, { 1, 1 } );
	//Input::Initialize();
	//CGE::ShowFPS( true );
	//CGE::SetTargetFPS( 0.0f );
	//ScreenBuffer::BlendingEnabled = true;
	//Cube  cube;
	//Plane plane;
	//Axes  axes;

	//Vector3 CubePosition = Vector3( 0.0f, 0.5f, 0.0f );
	//Vector3 CubeRotation = Vector3::Zero;
	//Vector3 CubeScale = Vector3::One * 1.0f;

	//Vector3 PlanePosition = Vector3( 0, 0, 0 );
	//Vector3 PlaneRotation = Vector3::Zero;
	//Vector3 PlaneScale = Vector3::One * 5.0f;

	//Vector3 AxesPosition = Vector3::Left * 5.0f;
	//Vector3 AxesRotation = Vector3::Zero;
	//Vector3 AxesScale = Vector3::One * 3.0f;

	//Vector3 CameraPosition = Vector3( 0.0f, 0.0f, -3.0f );
	//Quaternion CameraRotation;

	//auto ProjectionMatrix = Matrix4::CreateProjection( Math::Radians( 75.0f ), 1.0f );
	//auto ViewMatrix = Matrix4();
	//auto PVMatrix = Matrix4();
	//Vector3 forward = Vector3::Forward;
	//Vector3 right = Vector3::Right;
	//Vector3 up = Vector3::Up;

	//auto drawCube = [&]()
	//{
	//	Matrix4 CubeMatrix = Matrix4::CreateTransform( CubePosition, CubeRotation, CubeScale );
	//	auto PVM = Math::Multiply( PVMatrix, CubeMatrix );
	//	//auto PVM = Math::Multiply( CubeMatrix, PVMatrix );
	//	Vector4 Verts[ 8 ];

	//	for ( int i = 0; i < 8; ++i )
	//	{
	//		Verts[ i ] = Math::Multiply( PVM, cube.Corners[ i ] );
	//		//Verts[ i ] = Math::Multiply( cube.Corners[ i ], PVM );

	//		Verts[ i ] /= Verts[ i ].w;
	//		//Verts[ i ].z = Math::Max( Verts[ i ].z, 0.0f );
	//		Verts[ i ] *= Vector4( 64, 64, 1, 1 );
	//		Verts[ i ] += Vector4( 64, 64, 0, 0 );
	//	}

	//	Primitive::DrawTriangle( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 0 ].ToVector2(), Verts[ 2 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 1 ].ToVector2(), Verts[ 5 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 1 ].ToVector2(), Verts[ 6 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 5 ].ToVector2(), Verts[ 4 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 5 ].ToVector2(), Verts[ 7 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 4 ].ToVector2(), Verts[ 0 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 4 ].ToVector2(), Verts[ 3 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 4 ].ToVector2(), Verts[ 5 ].ToVector2(), Verts[ 1 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 4 ].ToVector2(), Verts[ 1 ].ToVector2(), Verts[ 0 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 3 ].ToVector2(), Verts[ 2 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 3 ].ToVector2(), Verts[ 6 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );

	//	Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 1 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 2 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 3 ].ToVector2(), Verts[ 0 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 4 ].ToVector2(), Verts[ 5 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 5 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 6 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 7 ].ToVector2(), Verts[ 4 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 4 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 1 ].ToVector2(), Verts[ 5 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 2 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour::DARK_GREY );
	//	Primitive::DrawLine( Verts[ 3 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour::DARK_GREY );
	//};

	//auto drawPlane = [&]()
	//{
	//	Matrix4 PlaneMatrix = Matrix4::CreateTransform( PlanePosition, PlaneRotation, PlaneScale );
	//	auto PVM = Math::Multiply( PVMatrix, PlaneMatrix );
	//	//auto PVM = Math::Multiply( PlaneMatrix, PVMatrix );
	//	Vector4 Verts[ 36 ];

	//	for ( int i = 0; i < 36; ++i )
	//	{
	//		Verts[ i ] = Math::Multiply( PVM, plane.Corners[ i ] );
	//		//Verts[ i ] = Math::Multiply( plane.Corners[ i ], PVM );

	//		Verts[ i ] /= Verts[ i ].w;
	//		//Verts[ i ].z = Math::Max( Verts[ i ].z, 0.0f );
	//		Verts[ i ] *= Vector4( 64, 64, 1, 1 );
	//		Verts[ i ] += Vector4( 64, 64, 0, 0 );
	//	}

	//	/*for ( int i = 0; i < 35; i += 2 )
	//	{
	//		Primitive::DrawLine( Verts[ i ].ToVector2(), Verts[ i + 1 ].ToVector2(), Colour::RED );
	//	}*/

	//	Primitive::DrawTriangle( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Verts[ 17 ].ToVector2(), Colour( Colour::LIGHT_PINK, 100 ) );
	//	Primitive::DrawTriangle( Verts[ 0 ].ToVector2(), Verts[ 17 ].ToVector2(), Verts[ 16 ].ToVector2(), Colour( Colour::LIGHT_PINK, 100 ) );
	//};

	//auto drawAxes = [&]()
	//{
	//	Matrix4 AxesMatrix = Matrix4::CreateTransform( AxesPosition, AxesRotation, AxesScale );
	//	auto PVM = Math::Multiply( PVMatrix, AxesMatrix );
	//	//auto PVM = Math::Multiply( PlaneMatrix, PVMatrix );
	//	Vector4 Verts[ 4 ];

	//	for ( int i = 0; i < 4; ++i )
	//	{
	//		Verts[ i ] = Math::Multiply( PVM, axes.Corners[ i ] );
	//		//Verts[ i ] = Math::Multiply( plane.Corners[ i ], PVM );

	//		Verts[ i ] /= Verts[ i ].w;
	//		//Verts[ i ].z = Math::Max( Verts[ i ].z, 0.0f );
	//		Verts[ i ] *= Vector4( 64, 64, 1, 1 );
	//		Verts[ i ] += Vector4( 64, 64, 0, 0 );
	//	}

	//	Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Colour::RED );
	//	Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour::GREEN );
	//	Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour::BLUE );
	//};

	//float movement = 1.0f;
	//float sensitivity = 0.0000001f;
	//float i = 0;
	//CGE::Run( [&]()
	//	{
	//		ViewMatrix = Matrix4::CreateView( CameraPosition, CameraRotation );
	//		PVMatrix = Math::Multiply( ProjectionMatrix, ViewMatrix );

	//		ScreenBuffer::SetBuffer( Colour::BLACK );
	//		//Primitive::DrawTriangle( Vector2( 60, 63 ), Vector2( 63.0f + 50.0f * Math::Cos( i ), 63.0f + 50.0f * Math::Sin( i+= 0.01f ) ), Vector2( 66, 63 ), Colour::GREEN );
	//		//Primitive::DrawTriangle( Vector2( 20, 50 ), Vector2( 80, 50 ), Vector2( 50, 80 ), Colour::GREEN );
	//		drawPlane();
	//		drawCube();
	//		drawAxes();

	//		right = ViewMatrix.r0.ToVector();
	//		up = ViewMatrix.r1.ToVector();
	//		forward = ViewMatrix.r2.ToVector();

	//		// Left
	//		if ( Input::IsKeyDown( KeyCode::A ) )
	//		{
	//			CameraPosition -= movement * right * Time::GetDeltaTime();
	//		}

	//		// Right
	//		if ( Input::IsKeyDown( KeyCode::D ) )
	//		{
	//			CameraPosition += movement * right * Time::GetDeltaTime();
	//		}

	//		// Forward
	//		if ( Input::IsKeyDown( KeyCode::W ) )
	//		{
	//			CameraPosition += movement * forward * Time::GetDeltaTime();
	//		}

	//		// Back
	//		if ( Input::IsKeyDown( KeyCode::S ) )
	//		{
	//			CameraPosition -= movement * forward * Time::GetDeltaTime();
	//		}

	//		// Up
	//		if ( Input::IsKeyDown( KeyCode::E ) )
	//		{
	//			CameraPosition += movement * Time::GetDeltaTime() * up;
	//		}

	//		// Down
	//		if ( Input::IsKeyDown( KeyCode::Q ) )
	//		{
	//			CameraPosition -= movement * up * Time::GetDeltaTime();
	//		}

	//		if ( Input::IsKeyDown( KeyCode::Esc ) )
	//		{
	//			CGE::Quit();
	//		}

	//		if ( Input::IsKeyDown( KeyCode::Shift ) )
	//		{
	//			movement = 4.0f;
	//		}
	//		else
	//		{
	//			movement = 1.0f;
	//		}

	//		CubeRotation.x += 1.0f * Time::GetDeltaTime();
	//		CubeRotation.y += 0.1f * Time::GetDeltaTime();

	//		if ( Input::IsMouseDown( MouseCode::RightMouse ) )
	//		{
	//			Vector2 MouseDelta = Input::GetMouseDelta();
	//			CameraRotation.y -= Math::Radians( MouseDelta.x * 1.0f );
	//			CameraRotation.x -= Math::Radians( MouseDelta.y * 1.0f );
	//		}

	//	});

	
	return 0;
}