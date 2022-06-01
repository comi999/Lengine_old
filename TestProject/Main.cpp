#include "Texture.hpp"
#include "Mesh.hpp"
#include "Primitive.hpp"
#include "entt/entt.hpp"
#include "Input.hpp"
#include "Component.hpp"
#include "Light.hpp"
#include "MeshRenderer.hpp"
#include "Transform.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "File.hpp"
#include "Resource.hpp"
#include "ConsoleWindow.hpp"
#include "Rendering.hpp"

#include <thread>
#include <iostream>

void Shader_Vertex()
{
	Rendering::In< 0, Vector2 > i_Position;
	Rendering::In< 1, Vector3 > i_Colour;
	Rendering::Out< 0, Vector4 > o_Colour;

	Rendering::Position = Vector4( *i_Position, 0.0f, 1.0f );
	*o_Colour = Vector4( *i_Colour, 1.0f );

	//std::cout << "X: " << _Position->x << ", Y: " << _Position->y << std::endl;
	//std::cout << "R: " << _Colour->x << ", G: " << _Colour->y << ", B: " << _Colour->z << std::endl;
}
void Shader_Fragment()
{
	Rendering::In< 0, Vector2 > _Position;
	Rendering::In< 1, Vector3 > _Colour;

	Rendering::FragColour = Vector4( 1, 0, 0, 1 );
}

ShaderProgramHandle LoadShaders( void* a_VertexShader, void* a_FragmentShader )
{
	ShaderHandle VertexShaderID = Rendering::CreateShader( ShaderType::VERTEX_SHADER );
	ShaderHandle FragmentShaderID = Rendering::CreateShader( ShaderType::FRAGMENT_SHADER );

	bool Result = false;
	int InfoLogLength;

	const void* VertexSourcePointer = a_VertexShader;
	const void* FragmentSourcePointer = a_FragmentShader;

	Rendering::ShaderSource( VertexShaderID, 1, &VertexSourcePointer, NULL );
	Rendering::CompileShader( VertexShaderID );
	Rendering::GetShaderIV( VertexShaderID, ShaderInfo::COMPILE_STATUS, &Result );
	Rendering::GetShaderIV( VertexShaderID, ShaderInfo::INFO_LOG_LENGTH, &InfoLogLength );

	/*if ( InfoLogLength > 0 )
	{
		std::vector<char> VertexShaderErrorMessage( InfoLogLength + 1 );
		Rendering::GetShaderInfoLog( VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[ 0 ] );
		printf( "%s\n", &VertexShaderErrorMessage[ 0 ] );
	}*/

	Rendering::ShaderSource( FragmentShaderID, 1, &FragmentSourcePointer, NULL );
	Rendering::CompileShader( FragmentShaderID );
	Rendering::GetShaderIV( FragmentShaderID, ShaderInfo::COMPILE_STATUS, &Result );
	Rendering::GetShaderIV( FragmentShaderID, ShaderInfo::INFO_LOG_LENGTH, &InfoLogLength );

	/*if ( InfoLogLength > 0 )
	{
		std::vector<char> FragmentShaderErrorMessage( InfoLogLength + 1 );
		Rendering::GetShaderInfoLog( FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[ 0 ] );
		printf( "%s\n", &FragmentShaderErrorMessage[ 0 ] );
	}*/

	ShaderProgramHandle ProgramID = Rendering::CreateProgram();
	Rendering::AttachShader( ProgramID, VertexShaderID );
	Rendering::AttachShader( ProgramID, FragmentShaderID );
	Rendering::LinkProgram( ProgramID );
	Rendering::GetProgramIV( ProgramID, ShaderInfo::LINK_STATUS, &Result );
	Rendering::GetProgramIV( ProgramID, ShaderInfo::INFO_LOG_LENGTH, &InfoLogLength );

	/*if ( InfoLogLength > 0 )
	{
		std::vector<char> ProgramErrorMessage( InfoLogLength + 1 );
		Rendering::GetProgramInfoLog( ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[ 0 ] );
		printf( "%s\n", &ProgramErrorMessage[ 0 ] );
	}*/

	Rendering::DetachShader( ProgramID, VertexShaderID );
	Rendering::DetachShader( ProgramID, FragmentShaderID );
	Rendering::DeleteShader( VertexShaderID );
	Rendering::DeleteShader( FragmentShaderID );

	return ProgramID;
}

int main()
{
	PixelColourMap::Initialize();
	
	auto* window = ConsoleWindow::Create( "SomeTitle", { 64, 64 }, { 8, 8 } );
	ConsoleWindow::MakeContextCurrent( window );

	ArrayHandle vao;
	Rendering::GenVertexArrays( 1, &vao );
	Rendering::BindVertexArray( vao );

	BufferHandle vbo;
	Rendering::GenBuffers( 1, &vbo );
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo );

	float vertices[] = {
		-0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f
	};

	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, sizeof( vertices ), vertices, DataUsage::STATIC_DRAW );

	Rendering::VertexAttribPointer( 0, 2, DataType::FLOAT, false, 5 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 0 );

	Rendering::VertexAttribPointer( 1, 3, DataType::FLOAT, false, 5 * sizeof( float ), ( void* )( 2 * sizeof( float ) ) );
	Rendering::EnableVertexAttribArray( 1 );

	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, 0 );
	Rendering::BindVertexArray( 0 );

	ShaderProgramHandle shader = LoadShaders( Shader_Vertex, Shader_Fragment );
	float i = 0.0f;
	while ( 1 )
	{
		vertices[ 0 ] = 0.5f * Math::Sin( i += 0.1f );
		Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo );
		Rendering::BufferData( BufferTarget::ARRAY_BUFFER, sizeof( vertices ), vertices, DataUsage::STATIC_DRAW );
		Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, 0 );

		Sleep( 66 );
		Rendering::ClearColour( 0.0f, 0.0f, 0.0f, 0.0f );
		Rendering::Clear( BufferFlag::COLOUR_BUFFER_BIT );
		Rendering::UseProgram( shader );
		Rendering::BindVertexArray( vao );
		Rendering::DrawArrays( RenderMode::TRIANGLE, 0, 6 );
		Rendering::BindVertexArray( 0 );
		ConsoleWindow::SwapBuffers( window );
	}

	//CGE::Initialize( "Some title", { 64, 64 }, { 1, 1 } );

	//auto landscape = Resource::GetOrLoad< Texture >( "grass_diffuse" );
	//auto grass = Resource::GetOrLoad< Mesh >( "grass" );

	//Input::Initialize();
	//CGE::ShowFPS( true );
	//CGE::SetTargetFPS( 0.0f );
	//ScreenBuffer::BlendingEnabled = true;

	//GameObject CubeObject = GameObject::Instantiate( "Cube"_N );
	//CubeObject.GetTransform()->SetGlobalScale( Vector3::One * 1.0f );
	//CubeObject.GetTransform()->SetGlobalPosition( Vector3::Up );
	//auto CubeRenderer = CubeObject.AddComponent< MeshRenderer >();
	//CubeRenderer->SetMesh( grass );
	//CubeRenderer->SetTexture( landscape );
	//

	//GameObject SubCubeObject = GameObject::Instantiate( "SubCube"_N );
	//SubCubeObject.GetTransform()->SetParent( CubeObject.GetTransform() );
	//SubCubeObject.GetTransform()->SetLocalPosition( Vector3::Left );
	//SubCubeObject.GetTransform()->SetLocalScale( Vector3::One * 0.5f );

	//GameObject PlaneObject = GameObject::Instantiate( "Plane"_N );
	//PlaneObject.GetTransform()->SetGlobalScale( Vector3::One * 10.0f );
	//PlaneObject.AddComponent< MeshRenderer >()->SetMesh( nullptr );

	//GameObject AxesObject = GameObject::Instantiate( "Axes"_N );
	//AxesObject.GetTransform()->SetGlobalScale( Vector3::One * 1.0f );
	//AxesObject.GetTransform()->SetGlobalPosition( Vector3::Left * 5.0f );

	//GameObject CameraObject = GameObject::Instantiate( "Camera"_N );
	//Camera* CameraComponent = CameraObject.AddComponent< Camera >();
	//CameraObject.GetTransform()->SetGlobalPosition( Vector3( 0.0f, 0.0f, -3.0f ) );
	//CameraObject.GetTransform()->SetGlobalRotation( Quaternion::ToQuaternion( Vector3::Zero ) );
	//CameraComponent->SetFOV( 60.0f );
	//CameraComponent->SetNearZ( 0.1f );
	//CameraComponent->SetFarZ( 1000.0f );
	//Camera::SetMainCamera( CameraComponent );

	//GameObject LightObject = GameObject::Instantiate( "Light"_N );
	//Light* LightComponent = LightObject.AddComponent< Light >();
	//LightComponent->SetDirection( Vector3( -1.0f, -1.0f, 1.0f ) );

	//auto PVMatrix = Matrix4();
	//Vector3 forward = Vector3::Forward;
	//Vector3 right = Vector3::Right;
	//Vector3 up = Vector3::Up;

	//#pragma endregion

	//float movement = 1.0f;
	//float sensitivity = 0.0000001f;
	//float i = 0;
	//float cubeRotation = 0.0f;
	//bool isAttached = true;
	//CGE::Run( [&]()
	//		  {
	//			  LightObject.GetComponent< Light >()->SetDirection( Vector3( Math::Cos( cubeRotation ), -0.1f, Math::Sin( cubeRotation ) ) );
	//			  CameraObject.GetTransform()->UpdateTransform();
	//			  PlaneObject.GetTransform()->UpdateTransform();
	//			  CubeObject.GetTransform()->UpdateTransform();
	//			  AxesObject.GetTransform()->UpdateTransform();

	//			  PVMatrix = CameraComponent->GetProjectionViewMatrix();

	//			  ScreenBuffer::SetBuffer( Colour::BLACK );

	//			  for ( int y = 0; y < ScreenBuffer::GetBufferHeight(); ++y )
	//			  {
	//				  for ( int x = 0; x < ScreenBuffer::GetBufferWidth(); ++x )
	//				  {
	//					  float U = float( x ) / ( ScreenBuffer::GetBufferWidth() - 1 );
	//					  float V = float( y ) / ( ScreenBuffer::GetBufferHeight() - 1 );
	//					  //ScreenBuffer::SetColour( { x, y }, landscape->Sample( { U, V } ) );
	//				  }
	//			  }

	//			  PlaneObject.GetComponent< MeshRenderer >()->Draw();
	//			  CubeObject.GetComponent< MeshRenderer >()->Draw();

	//			  right = CameraObject.GetTransform()->GetGlobalRight();
	//			  up = CameraObject.GetTransform()->GetGlobalUp();
	//			  forward = CameraObject.GetTransform()->GetGlobalForward();

	//			  // Left
	//			  if ( Input::IsKeyDown( KeyCode::A ) )
	//			  {
	//				  CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() - movement * right * Time::GetDeltaTime() );
	//			  }

	//			  // Right
	//			  if ( Input::IsKeyDown( KeyCode::D ) )
	//			  {
	//				  CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() + movement * right * Time::GetDeltaTime() );
	//			  }

	//			  // Forward
	//			  if ( Input::IsKeyDown( KeyCode::W ) )
	//			  {
	//				  CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() + movement * forward * Time::GetDeltaTime() );
	//			  }

	//			  // Back
	//			  if ( Input::IsKeyDown( KeyCode::S ) )
	//			  {
	//				  CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() - movement * forward * Time::GetDeltaTime() );
	//			  }

	//			  // Up
	//			  if ( Input::IsKeyDown( KeyCode::E ) )
	//			  {
	//				  CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() + movement * up * Time::GetDeltaTime() );
	//			  }

	//			  // Down
	//			  if ( Input::IsKeyDown( KeyCode::Q ) )
	//			  {
	//				  CameraObject.GetTransform()->SetGlobalPosition( CameraObject.GetTransform()->GetGlobalPosition() - movement * up * Time::GetDeltaTime() );
	//			  }

	//			  if ( Input::IsKeyDown( KeyCode::Esc ) )
	//			  {
	//				  CGE::Quit();
	//			  }

	//			  if ( Input::IsKeyDown( KeyCode::Shift ) )
	//			  {
	//				  movement = 10.0f;
	//			  }
	//			  else
	//			  {
	//				  movement = 4.0f;
	//			  }

	//			  Quaternion CubeRot = Quaternion::ToQuaternion( Math::Normalize( Vector3::One ), cubeRotation += Time::GetDeltaTime() );
	//			  //CubeObject.GetTransform()->SetLocalRotation( CubeRot );

	//			  if ( Input::IsMouseDown( MouseCode::RightMouse ) )
	//			  {
	//				  Vector2 MouseDelta = Input::GetMouseDelta();
	//				  Quaternion CameraRotation = CameraObject.GetTransform()->GetGlobalRotation();
	//				  Vector3 CameraEuler = Quaternion::ToEulerAngles( CameraRotation );
	//				  CameraEuler.y -= Math::Radians( MouseDelta.x * 1.0f );
	//				  CameraEuler.x -= Math::Radians( MouseDelta.y * 1.0f );
	//				  CameraObject.GetTransform()->SetGlobalRotation( Quaternion::ToQuaternion( CameraEuler ) );
	//			  }

	//			  //CameraObject.GetTransform()->SetGlobalForward( Math::Normalize( Vector3::Zero - CameraObject.GetTransform()->GetGlobalPosition() ) );
	//		  } );


	return 0;
}