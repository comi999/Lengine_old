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
#include "Math.hpp"
#include <thread>
#include <iostream>

#define GLM_LEFT_HANDED
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

DefineShader( Basic_Vertex )
{
	Uniform( Matrix4, PVM );
	Attribute( 0, Vector4, i_Position );
	Attribute( 1, Vector4, i_Colour );
	Varying_Out( Vector4, colour );

	Rendering::Position = Math::Multiply( PVM, i_Position );
	colour = i_Colour;
}

DefineShader( Basic_Fragment )
{
	Varying_In( Vector4, colour );

	Rendering::FragColour = colour;
}

DefineShader( Lit_Test_Vertex )
{
	Uniform( Matrix4, PVM );
	Uniform( Vector3, Light );

	Attribute( 0, Vector4, i_Position );
	Attribute( 1, Vector4, i_Colour );
	Attribute( 2, Vector3, i_Normal );

	Varying_Out( Vector3, lit );
	Varying_Out( Vector4, colour );

	colour = i_Colour;
	Rendering::Position = Math::Multiply( PVM, i_Position );
	Vector3 normal = Math::Multiply( PVM, Vector4( i_Normal, 1.0f ) );
	auto intensity = Math::Normalize( Light - Vector3( Rendering::Position ) );
	lit = Vector3( 0.5f * Math::Dot( intensity, Math::Normalize( normal ) ) + 0.5f );
}

DefineShader( Lit_Test_Fragment )
{
	Uniform( Vector3, Light );
	Varying_In( Vector3, lit );
	Varying_In( Vector4, colour );

	Rendering::FragColour = Vector4( lit * Vector3( colour ), 1.0f );
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

void RunCubeDemo()
{
	PixelColourMap::Initialize();

	auto* window = ConsoleWindow::Create( "Title", { 64, 64 }, { 8, 8 } );
	ConsoleWindow::MakeContextCurrent( window );
	Rendering::Init();
	ArrayHandle vao;
	Rendering::GenVertexArrays( 1, &vao );
	Rendering::BindVertexArray( vao );

	BufferHandle vbo[ 3 ];
	Rendering::GenBuffers( 3, vbo );

	Vector4 cube_positions[] = {
		{ +1, +1, -1, +1 },
		{ -1, +1, -1, +1 },
		{ -1, -1, -1, +1 },
		{ +1, -1, -1, +1 },
		{ +1, +1, +1, +1 },
		{ -1, +1, +1, +1 },
		{ -1, -1, +1, +1 },
		{ +1, -1, +1, +1 }
	};

	const Vector4 cube_colours[] = {
		{ 1, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 },
		{ 1, 0, 1, 1 },
		{ 0, 0, 1, 1 },
		{ 1, 1, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 0, 0, 1, 1 }
	};

	const Vector3 cube_normals[] = {
		{ +1, +1, -1 },
		{ -1, +1, -1 },
		{ -1, -1, -1 },
		{ +1, -1, -1 },
		{ +1, +1, +1 },
		{ -1, +1, +1 },
		{ -1, -1, +1 },
		{ +1, -1, +1 }
	};

	Vector4 vertices_positions[] = {
		cube_positions[ 0 ], cube_positions[ 1 ], cube_positions[ 2 ],
		cube_positions[ 0 ], cube_positions[ 2 ], cube_positions[ 3 ],
		cube_positions[ 1 ], cube_positions[ 5 ], cube_positions[ 6 ],
		cube_positions[ 1 ], cube_positions[ 6 ], cube_positions[ 2 ],
		cube_positions[ 5 ], cube_positions[ 4 ], cube_positions[ 7 ],
		cube_positions[ 5 ], cube_positions[ 7 ], cube_positions[ 6 ],
		cube_positions[ 4 ], cube_positions[ 0 ], cube_positions[ 3 ],
		cube_positions[ 4 ], cube_positions[ 3 ], cube_positions[ 7 ],
		cube_positions[ 4 ], cube_positions[ 5 ], cube_positions[ 1 ],
		cube_positions[ 4 ], cube_positions[ 1 ], cube_positions[ 0 ],
		cube_positions[ 3 ], cube_positions[ 2 ], cube_positions[ 6 ],
		cube_positions[ 3 ], cube_positions[ 6 ], cube_positions[ 7 ],
	};

	Vector4 vertices_colours[] = {
		cube_colours[ 0 ], cube_colours[ 1 ], cube_colours[ 2 ],
		cube_colours[ 0 ], cube_colours[ 2 ], cube_colours[ 3 ],
		cube_colours[ 1 ], cube_colours[ 5 ], cube_colours[ 6 ],
		cube_colours[ 1 ], cube_colours[ 6 ], cube_colours[ 2 ],
		cube_colours[ 5 ], cube_colours[ 4 ], cube_colours[ 7 ],
		cube_colours[ 5 ], cube_colours[ 7 ], cube_colours[ 6 ],
		cube_colours[ 4 ], cube_colours[ 0 ], cube_colours[ 3 ],
		cube_colours[ 4 ], cube_colours[ 3 ], cube_colours[ 7 ],
		cube_colours[ 4 ], cube_colours[ 5 ], cube_colours[ 1 ],
		cube_colours[ 4 ], cube_colours[ 1 ], cube_colours[ 0 ],
		cube_colours[ 3 ], cube_colours[ 2 ], cube_colours[ 6 ],
		cube_colours[ 3 ], cube_colours[ 6 ], cube_colours[ 7 ],
	};

	Vector3 vertices_normals[] = {
		cube_normals[ 0 ], cube_normals[ 1 ], cube_normals[ 2 ],
		cube_normals[ 0 ], cube_normals[ 2 ], cube_normals[ 3 ],
		cube_normals[ 1 ], cube_normals[ 5 ], cube_normals[ 6 ],
		cube_normals[ 1 ], cube_normals[ 6 ], cube_normals[ 2 ],
		cube_normals[ 5 ], cube_normals[ 4 ], cube_normals[ 7 ],
		cube_normals[ 5 ], cube_normals[ 7 ], cube_normals[ 6 ],
		cube_normals[ 4 ], cube_normals[ 0 ], cube_normals[ 3 ],
		cube_normals[ 4 ], cube_normals[ 3 ], cube_normals[ 7 ],
		cube_normals[ 4 ], cube_normals[ 5 ], cube_normals[ 1 ],
		cube_normals[ 4 ], cube_normals[ 1 ], cube_normals[ 0 ],
		cube_normals[ 3 ], cube_normals[ 2 ], cube_normals[ 6 ],
		cube_normals[ 3 ], cube_normals[ 6 ], cube_normals[ 7 ],
	};

	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 0 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, sizeof( vertices_positions ), vertices_positions, DataUsage::STATIC_DRAW );

	Rendering::VertexAttribPointer( 0, 4, DataType::FLOAT, false, 4 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 0 );

	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 1 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, sizeof( vertices_colours ), vertices_colours, DataUsage::STATIC_DRAW );

	Rendering::VertexAttribPointer( 1, 4, DataType::FLOAT, false, 4 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 1 );

	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 2 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, sizeof( vertices_normals ), vertices_normals, DataUsage::STATIC_DRAW );

	Rendering::VertexAttribPointer( 2, 3, DataType::FLOAT, false, 3 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 2 );

	//-------------------------------------------------------------
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, 0 );
	Rendering::BindVertexArray( 0 );


	ShaderProgramHandle shader = LoadShaders( Shader_Basic_Vertex, Shader_Basic_Fragment );
	

	Rendering::UseProgram( shader );
	
	Vector3 LightPosition = Vector3::Up * 5.0f + Vector3::Left * 3.0f;
	auto LightLocation = Rendering::GetUniformLocation( shader, "Light" );

	auto RotationLocation = Rendering::GetUniformLocation( shader, "PVM" );

	Rendering::ClearColour( 0.0f, 0.0f, 0.0f, 0.0f );
	
	//Rendering::Enable( RenderSetting::CULL_FACE );
	Rendering::Enable( RenderSetting::DEPTH_TEST );
	Rendering::ClearDepth( 1000.0f );

	auto Proj = Matrix4::CreateProjection( Math::Radians( 75.0f ), 1.0f, 0.1f, 1000.0f );
	auto View = Matrix4::CreateLookAt( Vector3( 0.0f, 2.0f, -5.0f ), Vector3::Zero, Vector3::Up );
	Matrix4 PV = Math::Multiply( Proj, View );
	float i = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 0.0000000005;
	
	while ( 1 )
	{
		i += 0.1f;
		auto Modl = Matrix4::CreateTransform( Vector3( 0.0f, 0.0f, 0.0f ), Quaternion::ToQuaternion( Vector3( 0.0f, i, 0.0f ) ), Vector3::One );
		auto PVM = Math::Multiply( PV, Modl );
		Rendering::UniformMatrix4fv( RotationLocation, 1, false, &PVM[ 0 ] );

		LightPosition.x = 50.0f * Math::Cos( i );
		LightPosition.z = 50.0f * Math::Sin( i );
		LightPosition.y = 0.0f;
		//Rendering::Uniform3f( LightLocation, LightPosition.x, LightPosition.y, LightPosition.z );

		Sleep( 33 );
		Rendering::Clear( BufferFlag::COLOUR_BUFFER_BIT | BufferFlag::DEPTH_BUFFER_BIT );
		Rendering::BindVertexArray( vao );
		Rendering::DrawArrays( RenderMode::TRIANGLE, 0, 36 );
		Rendering::BindVertexArray( 0 );

		ConsoleWindow::SwapBuffers( window );
	}
}

void RunSquaresDemo()
{
	PixelColourMap::Initialize();

	auto* window = ConsoleWindow::Create( "Title", { 64, 64 }, { 8, 8 } );
	ConsoleWindow::MakeContextCurrent( window );
	Rendering::Init();
	ArrayHandle vao;
	Rendering::GenVertexArrays( 1, &vao );
	Rendering::BindVertexArray( vao );

	BufferHandle vbo[ 2 ];
	Rendering::GenBuffers( 2, vbo );

	const Vector4 cube_positions[] = {
		{ +1, +1, +1, +1 },
		{ -1, +1, +1, +1 },
		{ -1, -1, +1, +1 },
		{ +1, -1, +1, +1 },
		{ +1, +1, -1, +1 },
		{ -1, +1, -1, +1 },
		{ -1, -1, -1, +1 },
		{ +1, -1, -1, +1 }
	};

	const Vector4 cube_colours[] = {
		{ 1, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 },
		{ 1, 0, 1, 1 },
		{ 0, 0, 1, 1 },
		{ 1, 1, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 0, 0, 1, 1 }
	};

	Vector4 vertices_positions[] = {
		cube_positions[ 0 ], cube_positions[ 1 ], cube_positions[ 2 ],
		cube_positions[ 0 ], cube_positions[ 2 ], cube_positions[ 3 ],
		cube_positions[ 1 ], cube_positions[ 5 ], cube_positions[ 6 ],
		cube_positions[ 1 ], cube_positions[ 6 ], cube_positions[ 2 ],
		cube_positions[ 5 ], cube_positions[ 4 ], cube_positions[ 7 ],
		cube_positions[ 5 ], cube_positions[ 7 ], cube_positions[ 6 ],
		cube_positions[ 4 ], cube_positions[ 0 ], cube_positions[ 3 ],
		cube_positions[ 4 ], cube_positions[ 3 ], cube_positions[ 7 ],
		cube_positions[ 4 ], cube_positions[ 5 ], cube_positions[ 1 ],
		cube_positions[ 4 ], cube_positions[ 1 ], cube_positions[ 0 ],
		cube_positions[ 3 ], cube_positions[ 2 ], cube_positions[ 6 ],
		cube_positions[ 3 ], cube_positions[ 6 ], cube_positions[ 7 ],
	};

	Vector4 vertices_colours[] = {
		cube_colours[ 0 ], cube_colours[ 1 ], cube_colours[ 2 ],
		cube_colours[ 0 ], cube_colours[ 2 ], cube_colours[ 3 ],
		cube_colours[ 1 ], cube_colours[ 5 ], cube_colours[ 6 ],
		cube_colours[ 1 ], cube_colours[ 6 ], cube_colours[ 2 ],
		cube_colours[ 5 ], cube_colours[ 4 ], cube_colours[ 7 ],
		cube_colours[ 5 ], cube_colours[ 7 ], cube_colours[ 6 ],
		cube_colours[ 4 ], cube_colours[ 0 ], cube_colours[ 3 ],
		cube_colours[ 4 ], cube_colours[ 3 ], cube_colours[ 7 ],
		cube_colours[ 4 ], cube_colours[ 5 ], cube_colours[ 1 ],
		cube_colours[ 4 ], cube_colours[ 1 ], cube_colours[ 0 ],
		cube_colours[ 3 ], cube_colours[ 2 ], cube_colours[ 6 ],
		cube_colours[ 3 ], cube_colours[ 6 ], cube_colours[ 7 ],
	};

	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 0 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, sizeof( vertices_positions ), vertices_positions, DataUsage::STATIC_DRAW );

	Rendering::VertexAttribPointer( 0, 4, DataType::FLOAT, false, 4 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 0 );

	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 1 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, sizeof( vertices_colours ), vertices_colours, DataUsage::STATIC_DRAW );

	Rendering::VertexAttribPointer( 1, 4, DataType::FLOAT, false, 4 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 1 );

	//-------------------------------------------------------------
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, 0 );
	Rendering::BindVertexArray( 0 );

	ShaderProgramHandle shader = LoadShaders( Shader_Basic_Vertex, Shader_Basic_Fragment );

	auto RotationLocation = Rendering::GetUniformLocation( shader, "PVM" );
	Rendering::UseProgram( shader );

	//Rendering::Enable( RenderSetting::CULL_FACE );
	Rendering::Enable( RenderSetting::DEPTH_TEST );
	Rendering::ClearDepth( 0.00000f );
	Rendering::ClearColour( 0.0f, 0.0f, 0.0f, 0.0f );

	float rotation = 0.0f;
	auto Proj = Matrix4::CreateProjection( Math::Radians( 75.0f ), 1.0f );
	auto View = Matrix4::CreateLookAt( Vector3( 10.0f, 2.0f, -5.0f ), Vector3::Zero, Vector3::Up );

	Matrix4 PV = Math::Multiply( Proj, View );
	float i = 0.0f;
	while ( 1 )
	{
		i += 0.1f;
		auto Modl = Matrix4::CreateTransform( Vector3( Math::Cos( i ), 0.0f, 0.0f ), Quaternion::ToQuaternion( Vector3( 0.0f, i, 0.0f ) ), Vector3::One );
		auto PVM = Math::Multiply( PV, Modl );
		Rendering::UniformMatrix4fv( RotationLocation, 1, false, &PVM[ 0 ] );

		Sleep( 33 );
		Rendering::Clear( BufferFlag::COLOUR_BUFFER_BIT | BufferFlag::DEPTH_BUFFER_BIT );
		Rendering::BindVertexArray( vao );
		Rendering::DrawArrays( RenderMode::TRIANGLE, 0, 36 );
		Rendering::BindVertexArray( 0 );

		ConsoleWindow::SwapBuffers( window );
	}
}

void RunOldDemo()
{
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
}

int main()
{
	
	RunCubeDemo();
	//RunSquaresDemo();
	//RunOldDemo();
	


	return 0;
}