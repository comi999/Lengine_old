#include "CGE.hpp"
#include "Component.hpp"
#include "GameObject.hpp"
#include "Prefab.hpp"
#include "CameraController.hpp"

void InitializeScene()
{
	auto prefab = Resource::Load< Prefab >( "spear"_H );
	GameObject object = Prefab::Instantiate( *prefab );
	MeshRenderer* renderer = object.GetComponentInChild< MeshRenderer >();
	renderer->GetMaterial()->SetShader( Shader::Diffuse );
	object.GetTransform()->SetGlobalScale( Vector3::One );
	GameObject SunObject = GameObject::Instantiate( "Sun"_N );
	Light* LightComponent = SunObject.AddComponent< Light >();
	Light::SetSun( LightComponent );

	GameObject CameraObject = GameObject::Instantiate( "Camera"_N );
	Camera* CameraComponent = CameraObject.AddComponent< Camera >();
	CameraComponent->SetAspect( 1.0f );
	CameraComponent->SetFOV( Math::Radians( 75.0f ) );
	CameraComponent->SetNearZ( 0.1f );
	CameraComponent->SetFarZ( 100.0f );
	Camera::SetMainCamera( CameraComponent );
}

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <sstream>

GLuint LoadShaders( const char* vertex_file_path, const char* fragment_file_path )
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
	GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream( vertex_file_path, std::ios::in );
	if ( VertexShaderStream.is_open() )
	{
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else
	{
		printf( "Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path );
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream( fragment_file_path, std::ios::in );
	if ( FragmentShaderStream.is_open() )
	{
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf( "Compiling shader : %s\n", vertex_file_path );
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource( VertexShaderID, 1, &VertexSourcePointer, NULL );
	glCompileShader( VertexShaderID );

	// Check Vertex Shader
	glGetShaderiv( VertexShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 )
	{
		std::vector<char> VertexShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[ 0 ] );
		printf( "%s\n", &VertexShaderErrorMessage[ 0 ] );
	}

	// Compile Fragment Shader
	printf( "Compiling shader : %s\n", fragment_file_path );
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource( FragmentShaderID, 1, &FragmentSourcePointer, NULL );
	glCompileShader( FragmentShaderID );

	// Check Fragment Shader
	glGetShaderiv( FragmentShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 )
	{
		std::vector<char> FragmentShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[ 0 ] );
		printf( "%s\n", &FragmentShaderErrorMessage[ 0 ] );
	}

	// Link the program
	printf( "Linking program\n" );
	GLuint ProgramID = glCreateProgram();
	glAttachShader( ProgramID, VertexShaderID );
	glAttachShader( ProgramID, FragmentShaderID );
	glLinkProgram( ProgramID );

	// Check the program
	glGetProgramiv( ProgramID, GL_LINK_STATUS, &Result );
	glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 )
	{
		std::vector<char> ProgramErrorMessage( InfoLogLength + 1 );
		glGetProgramInfoLog( ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[ 0 ] );
		printf( "%s\n", &ProgramErrorMessage[ 0 ] );
	}

	glDetachShader( ProgramID, VertexShaderID );
	glDetachShader( ProgramID, FragmentShaderID );

	glDeleteShader( VertexShaderID );
	glDeleteShader( FragmentShaderID );

	return ProgramID;
}

int RunCubeTest()
{
	glfwInit();
	GLFWwindow* window = nullptr;

	if ( !( window = glfwCreateWindow( 1000, 1000, "hi", NULL, NULL ) ) )
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent( window );

	if ( glewInit() )
	{
		return 0;
	}

	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	GLuint vbo[ 3 ];
	glGenBuffers( 3, vbo );

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

	const uint32_t cube_indices[] = {
		0, 1, 2,
		0, 2, 3,
		1, 5, 6,
		1, 6, 2,
		5, 4, 7, 
		5, 7, 6,
		4, 0, 3,
		4, 3, 7,
		4, 5, 1,
		4, 1, 0,
		3, 2, 6,
		3, 6, 7
	};

	/*Vector4 vertices_positions[] = {
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
	};*/

	glBindBuffer( GL_ARRAY_BUFFER, vbo[ 0 ] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( cube_positions ), cube_positions, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 4, GL_FLOAT, false, 4 * sizeof( float ), ( void* )0 );
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[ 1 ] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( cube_colours ), cube_colours, GL_STATIC_DRAW );

	glVertexAttribPointer( 1, 4, GL_FLOAT, false, 4 * sizeof( float ), ( void* )0 );
	glEnableVertexAttribArray( 1 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo[ 2 ] );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( cube_indices ), cube_indices, GL_STATIC_DRAW );

	//-------------------------------------------------------------
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	GLuint shader = LoadShaders( "shader.vert", "shader.frag" );

	auto RotationLocation = glGetUniformLocation( shader, "PVM" );
	glUseProgram( shader );

	//glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glActiveTexture( GL_TEXTURE9 );

	auto Proj = Matrix4::CreateProjection( Math::Radians( 75.0f ), 1.0f, 0.1f, 1000.0f );
	auto View = Matrix4::CreateLookAt( Vector3{ 0.0f, 2.0f, -5.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 1.0f, 0.0f } );
	auto PV = Math::Multiply( Proj, View );
	float i = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 0.0000000005;

	while ( 1 )
	{
		i += 0.1f;
		auto mat = Matrix4::Identity;
		Matrix4::Rotate( mat, Quaternion::ToQuaternion( Vector3{ 0.0f, i, 0.0f } ) );
		Matrix4::Translate( mat, Vector3{ Math::Cos( i ), 0.0f, 0.0f } );
		auto Modl = mat;
		auto PVM = Math::Multiply( PV, Modl );

		glUniformMatrix4fv( RotationLocation, 1, false, &PVM[ 0 ] );
		Sleep( 33 );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glBindVertexArray( vao );
		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, ( void* )0 );
		glBindVertexArray( 0 );

		glfwSwapBuffers( window );
	}
}










int main()
{
	//RunCubeTest();



	auto Window = ConsoleWindow::Create( "Title goes here.", { 64, 64 }, { 16, 16 } );
	ConsoleWindow::MakeContextCurrent( Window );
	CGE::Init();

	InitializeScene();

	GameObject CameraObject = *Camera::GetMainCamera()->GetOwner();
	CameraObject.GetTransform()->SetLocalPosition( Vector3( 0.0f, 2.0f, -3.0f ) );
	CameraObject.AddComponent< CameraController >();

	Action<> GameLoop = [&]()
	{
		CameraObject.GetComponent< CameraController >()->Update();
	};

	CGE::Run( GameLoop );
	return 0;
}