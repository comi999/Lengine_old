#include "Primitive.hpp"
#include "entt/entt.hpp"
#include "Input.hpp"
#include "Component.hpp"
#include "Light.hpp"
#include "MeshRenderer.hpp"

//struct Cube
//{
//	Vector4 Corners[ 8 ]
//	{
//		Vector4( 0.5f,  0.5f, -0.5f, 1.0f ),
//		Vector4( -0.5f,  0.5f, -0.5f, 1.0f ),
//		Vector4( -0.5f, -0.5f, -0.5f, 1.0f ),
//		Vector4( 0.5f, -0.5f, -0.5f, 1.0f ),
//		Vector4( 0.5f,  0.5f,  0.5f, 1.0f ),
//		Vector4( -0.5f,  0.5f,  0.5f, 1.0f ),
//		Vector4( -0.5f, -0.5f,  0.5f, 1.0f ),
//		Vector4( 0.5f, -0.5f,  0.5f, 1.0f ),
//	};
//};
//
//struct Plane
//{
//	Vector4 Corners[ 36 ]
//	{
//		Vector4( -0.500f, 0.000f, -0.500f, 1.000f ), Vector4( -0.500f, 0.000f,  0.500f, 1.000f ),
//		Vector4( -0.375f, 0.000f, -0.500f, 1.000f ), Vector4( -0.375f, 0.000f,  0.500f, 1.000f ),
//		Vector4( -0.250f, 0.000f, -0.500f, 1.000f ), Vector4( -0.250f, 0.000f,  0.500f, 1.000f ),
//		Vector4( -0.125f, 0.000f, -0.500f, 1.000f ), Vector4( -0.125f, 0.000f,  0.500f, 1.000f ),
//		Vector4(  0.000f, 0.000f, -0.500f, 1.000f ), Vector4(  0.000f, 0.000f,  0.500f, 1.000f ),
//		Vector4(  0.125f, 0.000f, -0.500f, 1.000f ), Vector4(  0.125f, 0.000f,  0.500f, 1.000f ),
//		Vector4(  0.250f, 0.000f, -0.500f, 1.000f ), Vector4(  0.250f, 0.000f,  0.500f, 1.000f ),
//		Vector4(  0.375f, 0.000f, -0.500f, 1.000f ), Vector4(  0.375f, 0.000f,  0.500f, 1.000f ),
//		Vector4(  0.500f, 0.000f, -0.500f, 1.000f ), Vector4(  0.500f, 0.000f,  0.500f, 1.000f ),
//		Vector4( -0.500f, 0.000f,-0.500f, 1.000f ), Vector4( 0.500f, 0.000f,-0.500f, 1.000f ),
//		Vector4( -0.500f, 0.000f,-0.375f, 1.000f ), Vector4( 0.500f, 0.000f,-0.375f, 1.000f ),
//		Vector4( -0.500f, 0.000f,-0.250f, 1.000f ), Vector4( 0.500f, 0.000f,-0.250f, 1.000f ),
//		Vector4( -0.500f, 0.000f,-0.125f, 1.000f ), Vector4( 0.500f, 0.000f,-0.125f, 1.000f ),
//		Vector4( -0.500f, 0.000f, 0.000f, 1.000f ), Vector4( 0.500f, 0.000f, 0.000f, 1.000f ),
//		Vector4( -0.500f, 0.000f, 0.125f, 1.000f ), Vector4( 0.500f, 0.000f, 0.125f, 1.000f ),
//		Vector4( -0.500f, 0.000f, 0.250f, 1.000f ), Vector4( 0.500f, 0.000f, 0.250f, 1.000f ),
//		Vector4( -0.500f, 0.000f, 0.375f, 1.000f ), Vector4( 0.500f, 0.000f, 0.375f, 1.000f ),
//		Vector4( -0.500f, 0.000f, 0.500f, 1.000f ), Vector4( 0.500f, 0.000f, 0.500f, 1.000f ),
//	};
//};
//
//struct Axes
//{
//	Vector4 Corners[ 4 ]
//	{
//		Vector4( Vector3::Zero, 1.0f ),
//		Vector4( Vector3::Right, 1.0f ),
//		Vector4( Vector3::Up, 1.0f ),
//		Vector4( Vector3::Forward, 1.0f )
//	};
//};

#include "Transform.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"

int main()
{
	CGE::Initialize( "Some title", { 128, 128 }, { 1, 1 } );
	Input::Initialize();
	CGE::ShowFPS( true );
	CGE::SetTargetFPS( 0.0f );
	ScreenBuffer::BlendingEnabled = true;
	/*Cube  cube;
	Plane plane;
	Axes  axes;*/

	GameObject CubeObject = GameObject::Instantiate( "Cube"_N );
	CubeObject.GetTransform()->SetGlobalScale( Vector3::One * 3.0f );
	CubeObject.GetTransform()->SetGlobalPosition( Vector3::Up );
	CubeObject.AddComponent< MeshRenderer >()->SetMesh( &Mesh::Cube );

	GameObject SubCubeObject = GameObject::Instantiate( "SubCube"_N );
	SubCubeObject.GetTransform()->SetParent( CubeObject.GetTransform() );
	SubCubeObject.GetTransform()->SetLocalPosition( Vector3::Left );
	SubCubeObject.GetTransform()->SetLocalScale( Vector3::One * 0.5f );

	GameObject PlaneObject = GameObject::Instantiate( "Plane"_N );
	PlaneObject.GetTransform()->SetGlobalScale( Vector3::One * 10.0f );
	PlaneObject.AddComponent< MeshRenderer >()->SetMesh( &Mesh::Plane );

	GameObject AxesObject = GameObject::Instantiate( "Axes"_N );
	AxesObject.GetTransform()->SetGlobalScale( Vector3::One * 1.0f );
	AxesObject.GetTransform()->SetGlobalPosition( Vector3::Left * 5.0f );

	GameObject CameraObject = GameObject::Instantiate( "Camera"_N );
	Camera* CameraComponent = CameraObject.AddComponent< Camera >();
	CameraObject.GetTransform()->SetGlobalPosition( Vector3( 0.0f, 0.0f, -3.0f ) );
	CameraObject.GetTransform()->SetGlobalRotation( Quaternion::ToQuaternion( Vector3::Zero ) );
	CameraComponent->SetFOV( 60.0f );
	CameraComponent->SetNearZ( 0.1f );
	CameraComponent->SetFarZ( 1000.0f );
	Camera::SetMainCamera( CameraComponent );

	GameObject LightObject = GameObject::Instantiate( "Light"_N );
	Light* LightComponent = LightObject.AddComponent< Light >();
	LightComponent->SetDirection( Vector3( -1.0f, -1.0f, 1.0f ) );

	auto PVMatrix = Matrix4();
	Vector3 forward = Vector3::Forward;
	Vector3 right = Vector3::Right;
	Vector3 up = Vector3::Up;

	/*auto drawCube = [&]( Transform* transform )
	{
		auto PVM = Math::Multiply( PVMatrix, transform->GetGlobalMatrix() );
		Vector4 Verts[ 8 ];

		for ( int i = 0; i < 8; ++i )
		{
			Verts[ i ] = Math::Multiply( PVM, cube.Corners[ i ] );
			Verts[ i ] /= Verts[ i ].w;
			Verts[ i ] *= Vector4( 64, 64, 1, 1 );
			Verts[ i ] += Vector4( 64, 64, 0, 0 );
		}

		Primitive::DrawTriangle( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 0 ].ToVector2(), Verts[ 2 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 1 ].ToVector2(), Verts[ 5 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 1 ].ToVector2(), Verts[ 6 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 5 ].ToVector2(), Verts[ 4 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 5 ].ToVector2(), Verts[ 7 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 4 ].ToVector2(), Verts[ 0 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 4 ].ToVector2(), Verts[ 3 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 4 ].ToVector2(), Verts[ 5 ].ToVector2(), Verts[ 1 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 4 ].ToVector2(), Verts[ 1 ].ToVector2(), Verts[ 0 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 3 ].ToVector2(), Verts[ 2 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );
		Primitive::DrawTriangle( Verts[ 3 ].ToVector2(), Verts[ 6 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour( Colour::LIGHT_RED, 100 ) );

		Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 1 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 2 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 3 ].ToVector2(), Verts[ 0 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 4 ].ToVector2(), Verts[ 5 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 5 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 6 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 7 ].ToVector2(), Verts[ 4 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 4 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 1 ].ToVector2(), Verts[ 5 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 2 ].ToVector2(), Verts[ 6 ].ToVector2(), Colour::DARK_GREY );
		Primitive::DrawLine( Verts[ 3 ].ToVector2(), Verts[ 7 ].ToVector2(), Colour::DARK_GREY );
	};

	auto drawPlane = [&]()
	{
		auto PVM = Math::Multiply( PVMatrix, PlaneObject.GetTransform()->GetGlobalMatrix() );
		Vector4 Verts[ 36 ];

		for ( int i = 0; i < 36; ++i )
		{
			Verts[ i ] = Math::Multiply( PVM, plane.Corners[ i ] );
			Verts[ i ] /= Verts[ i ].w;
			Verts[ i ] *= Vector4( 64, 64, 1, 1 );
			Verts[ i ] += Vector4( 64, 64, 0, 0 );
		}

		for ( int i = 0; i < 35; i += 2 )
		{
			Primitive::DrawLine( Verts[ i ].ToVector2(), Verts[ i + 1 ].ToVector2(), Colour::RED );
		}

		Primitive::DrawTriangle( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Verts[ 17 ].ToVector2(), Colour( Colour::LIGHT_PINK, 100 ) );
		Primitive::DrawTriangle( Verts[ 0 ].ToVector2(), Verts[ 17 ].ToVector2(), Verts[ 16 ].ToVector2(), Colour( Colour::LIGHT_PINK, 100 ) );
	};

	auto drawAxes = [&]()
	{
		auto PVM = Math::Multiply( PVMatrix, AxesObject.GetTransform()->GetGlobalMatrix() );
		Vector4 Verts[ 4 ];

		for ( int i = 0; i < 4; ++i )
		{
			Verts[ i ] = Math::Multiply( PVM, axes.Corners[ i ] );
			Verts[ i ] /= Verts[ i ].w;
			Verts[ i ] *= Vector4( 64, 64, 1, 1 );
			Verts[ i ] += Vector4( 64, 64, 0, 0 );
		}

		Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 1 ].ToVector2(), Colour::RED );
		Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 2 ].ToVector2(), Colour::GREEN );
		Primitive::DrawLine( Verts[ 0 ].ToVector2(), Verts[ 3 ].ToVector2(), Colour::BLUE );
	};*/

	float movement = 1.0f;
	float sensitivity = 0.0000001f;
	float i = 0;
	float cubeRotation = 0.0f;
	bool isAttached = true;
	CGE::Run( [&]()
	{
		CameraObject.GetTransform()->UpdateTransform();
		PlaneObject.GetTransform()->UpdateTransform();
		CubeObject.GetTransform()->UpdateTransform();
		AxesObject.GetTransform()->UpdateTransform();

		PVMatrix = CameraComponent->GetProjectionViewMatrix();

		ScreenBuffer::SetBuffer( Colour::BLACK );
		/*drawPlane();
		drawCube( CubeObject.GetTransform() );
		drawCube( SubCubeObject.GetTransform() );
		drawAxes();*/
		
		PlaneObject.GetComponent< MeshRenderer >()->Draw();
		CubeObject.GetComponent< MeshRenderer >()->Draw();

		right = CameraObject.GetTransform()->GetGlobalRight();
		up = CameraObject.GetTransform()->GetGlobalUp();
		forward = CameraObject.GetTransform()->GetGlobalForward();

		// Left
		if ( Input::IsKeyDown( KeyCode::A ) )
		{
			CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() - movement * right * Time::GetDeltaTime() );
		}

		// Right
		if ( Input::IsKeyDown( KeyCode::D ) )
		{
			CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() + movement * right * Time::GetDeltaTime() );
		}

		// Forward
		if ( Input::IsKeyDown( KeyCode::W ) )
		{
			CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() + movement * forward * Time::GetDeltaTime() );
		}

		// Back
		if ( Input::IsKeyDown( KeyCode::S ) )
		{
			CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() - movement * forward * Time::GetDeltaTime() );
		}

		// Up
		if ( Input::IsKeyDown( KeyCode::E ) )
		{
			CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() + movement * up * Time::GetDeltaTime() );
		}

		// Down
		if ( Input::IsKeyDown( KeyCode::Q ) )
		{
			CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() - movement * up * Time::GetDeltaTime() );
		}

		if ( Input::IsKeyDown( KeyCode::Esc ) )
		{
			CGE::Quit();
		}

		if ( Input::IsKeyDown( KeyCode::Shift ) )
		{
			movement = 10.0f;
		}
		else
		{
			movement = 4.0f;
		}

		Quaternion CubeRot = Quaternion::ToQuaternion( Math::Normalize( Vector3::One ), cubeRotation += Time::GetDeltaTime() );
		CubeObject.GetTransform()->SetLocalRotation( CubeRot );
		
		if ( Input::IsMouseDown( MouseCode::RightMouse ) )
		{
			Vector2 MouseDelta = Input::GetMouseDelta();
			Quaternion CameraRotation = CameraObject.GetTransform()->GetGlobalRotation();
			Vector3 CameraEuler = Quaternion::ToEulerAngles( CameraRotation );
			CameraEuler.y -= Math::Radians( MouseDelta.x * 1.0f );
			CameraEuler.x -= Math::Radians( MouseDelta.y * 1.0f );
			CameraObject.GetTransform()->SetGlobalRotation( Quaternion::ToQuaternion( CameraEuler ) );
		}

		CameraObject.GetTransform()->SetGlobalForward( Math::Normalize( Vector3::Zero - CameraObject.GetTransform()->GetGlobalPosition() ) );
	} );

	
	return 0;
}