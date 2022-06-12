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

// Basic shaders
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

// Basic textured
DefineShader( Basic_Textured_Vertex )
{
	Uniform( Matrix4, PVM );
	Attribute( 0, Vector4, i_Position );
	Attribute( 3, Vector2, i_Texel );
	Varying_Out( Vector2, Texel );

	Rendering::Position = Math::Multiply( PVM, i_Position );
	Texel = i_Texel;
}

DefineShader( Basic_Textured_Fragment )
{
	Uniform( Sampler2D, DiffuseTexture );
	Varying_In( Vector2, Texel );

	Rendering::FragColour = Rendering::Sample( DiffuseTexture, Texel );
}

// Basic lit
DefineShader( Basic_Lit_Vertex )
{
	Uniform( Matrix4, PVM );
	Uniform( Matrix4, PV );
	Uniform( Matrix4, M );

	Attribute( 0, Vector3, i_Position );
	Attribute( 1, Vector2, i_Texel );
	Attribute( 2, Vector3, i_Normal );
	Attribute( 3, Vector3, i_Tangent );
	Attribute( 4, Vector3, i_Bitangent );

	Varying_Out( Vector4, position );
	Varying_Out( Vector2, texel );
	Varying_Out( Vector3, normal );
	Varying_Out( Vector3, tangent );
	Varying_Out( Vector3, bitangent );

	position = Math::Multiply( M, Vector4( i_Position, 1.0f ) );
	texel = i_Texel;
	normal = Math::Multiply( M, Vector4( i_Normal, 0.0f ) );
	tangent = Math::Multiply( M, Vector4( i_Tangent, 0.0f ) );
	bitangent = Math::Multiply( M, Vector4( i_Bitangent, 0.0f ) );
	Rendering::Position = Math::Multiply( PV, position );
}

DefineShader( Basic_Lit_Fragment )
{
	Uniform( Sampler2D, DiffuseTexture );
	Uniform( Sampler2D, NormalTexture );
	Uniform( Sampler2D, SpecularTexture );
	Uniform( Vector3, LightObject );
	Uniform( Vector4, LightColour );
	Uniform( Vector3, CameraObject );
	Uniform( Vector4, AmbientLight );

	Varying_In( Vector4, position );
	Varying_In( Vector2, texel );
	Varying_In( Vector3, normal );
	Varying_In( Vector3, tangent );
	Varying_In( Vector3, bitangent );

	Vector4 diffuseFrag  = Rendering::Sample( DiffuseTexture, texel );
	Vector4 normalFrag   = Rendering::Sample( NormalTexture, texel );
	Vector4 specularFrag = Rendering::Sample( SpecularTexture, texel );
	Matrix4 TBN( Vector4( Math::Normalize( normal ), 0.0f ), Vector4( Math::Normalize( tangent ), 0.0f ), Vector4( Math::Normalize( bitangent ), 0.0f ), Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
	Vector4 Normal = Math::Multiply( TBN, Vector4( normal, 0.0f ) * 2 - Vector4::One );
	Vector3 R = Math::Reflect( LightObject, Vector3( Normal ) );
	Vector3 V = Math::Normalize( CameraObject - LightObject );
	float LambertTerm = Math::Clamp( Math::Dot( Vector3( Normal ), -LightObject ), 0.0f, 1.0f );
	float SpecularTerm = Math::Pow( Math::Clamp( Math::Dot( R, V ), 0.0f, 1.0f ), 20.0f /*Ns*/ );
	Vector4 Diffuse = LightColour * /*Kd*/ Vector4( 0.8f, 0.8f, 0.8f, 1.0f ) * LambertTerm;
	Vector4 Specular = LightColour * /*Ks*/ Vector4( 0.5f, 0.5f, 0.5f, 1.0f ) * LambertTerm * SpecularTerm;
	Rendering::FragColour = Vector4( ( AmbientLight + Diffuse ) * diffuseFrag + Specular * specularFrag, 1.0f );
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

	auto landscape = Resource::GetOrLoad< Texture2D >( "wierd" );
	auto grass = Resource::GetOrLoad< Mesh >( "grass" );

	auto* window = ConsoleWindow::Create( "Title", { 92, 92 }, { 8, 8 } );
	ConsoleWindow::MakeContextCurrent( window );
	Rendering::Init();
	ArrayHandle vao;
	Rendering::GenVertexArrays( 1, &vao );
	Rendering::BindVertexArray( vao );

	BufferHandle vbo[ 4 ];
	Rendering::GenBuffers( 4, vbo );

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

	const Vector2 cube_texels[] = {
		{ 1, 1 },
		{ 0, 1 },
		{ 0, 0 },
		{ 1, 0 },

		{ 1, 1 },
		{ 0, 1 },
		{ 0, 0 },
		{ 1, 0 },
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

	Vector2 vertices_texels[] = {
		cube_texels[ 0 ], cube_texels[ 1 ], cube_texels[ 2 ],
		cube_texels[ 0 ], cube_texels[ 2 ], cube_texels[ 3 ],
		cube_texels[ 1 ], cube_texels[ 5 ], cube_texels[ 6 ],
		cube_texels[ 1 ], cube_texels[ 6 ], cube_texels[ 2 ],
		cube_texels[ 5 ], cube_texels[ 4 ], cube_texels[ 7 ],
		cube_texels[ 5 ], cube_texels[ 7 ], cube_texels[ 6 ],
		cube_texels[ 4 ], cube_texels[ 0 ], cube_texels[ 3 ],
		cube_texels[ 4 ], cube_texels[ 3 ], cube_texels[ 7 ],
		cube_texels[ 4 ], cube_texels[ 5 ], cube_texels[ 1 ],
		cube_texels[ 4 ], cube_texels[ 1 ], cube_texels[ 0 ],
		cube_texels[ 3 ], cube_texels[ 2 ], cube_texels[ 6 ],
		cube_texels[ 3 ], cube_texels[ 6 ], cube_texels[ 7 ],
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
	
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 3 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, sizeof( vertices_texels ), vertices_texels, DataUsage::STATIC_DRAW );

	Rendering::VertexAttribPointer( 3, 2, DataType::FLOAT, false, 2 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 3 );

	//-------------------------------------------------------------
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, 0 );
	Rendering::BindVertexArray( 0 );


	ShaderProgramHandle shader = LoadShaders( Shader_Basic_Textured_Vertex, Shader_Basic_Textured_Fragment );
	

	Rendering::UseProgram( shader );
	
	//Vector3 LightPosition = Vector3::Up * 5.0f + Vector3::Left * 3.0f;
	//auto LightLocation = Rendering::GetUniformLocation( shader, "Light" );

	auto RotationLocation = Rendering::GetUniformLocation( shader, "PVM" );
	auto DiffuseTextureLoc = Rendering::GetUniformLocation( shader, "DiffuseTexture" );

	TextureHandle tbo[ 1 ];
	Rendering::ActiveTexture( 0 );
	Rendering::GenTextures( 1, tbo );
	Rendering::BindTexture( TextureTarget::TEXTURE_2D, tbo[ 0 ] );
	Rendering::TexImage2D( TextureTarget::TEXTURE_2D, 0, TextureFormat( 0 ), landscape->GetWidth(), landscape->GetHeight(), 0, TextureFormat(0), TextureSetting( 0 ), landscape->GetData() );

	Rendering::Uniform1i( DiffuseTextureLoc, 0 );

	Rendering::ClearColour( 1.0f, 0.5f, 0.2f, 1.0f );
	
	Rendering::Enable( RenderSetting::CULL_FACE );
	//Rendering::Enable( RenderSetting::DEPTH_TEST );
	Rendering::ClearDepth( 1000.0f );

	auto Proj = Matrix4::CreateProjection( Math::Radians( 75.0f ), 1.0f, 0.1f, 1000.0f );
	auto View = Matrix4::CreateLookAt( Vector3( 0.0f, 2.0f, -5.0f ), Vector3::Zero, Vector3::Up );
	Matrix4 PV = Math::Multiply( Proj, View );
	float i = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 0.0000000005;
	
	while ( 1 )
	{
		i += 0.1f;
		auto Modl = Matrix4::CreateTransform( Vector3( 0.0f, 0.0f, 0.0f ), Quaternion::ToQuaternion( Vector3( 0.0f, i, 0.0f ) ), Vector3::One * 2.0f );
		auto PVM = Math::Multiply( PV, Modl );
		Rendering::UniformMatrix4fv( RotationLocation, 1, false, &PVM[ 0 ] );

		//LightPosition.x = 50.0f * Math::Cos( i );
		//LightPosition.z = 50.0f * Math::Sin( i );
		//LightPosition.y = 0.0f;
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

void RunLitSpearDemo()
{
	PixelColourMap::Initialize();
	auto* window = ConsoleWindow::Create( "Title", { 100, 100 }, { 8, 8 } );
	ConsoleWindow::MakeContextCurrent( window );
	Rendering::Init();

	// Load resources
	auto spear = Resource::GetOrLoad< Mesh >( "spear" );
	auto spear_diffuse = Resource::GetOrLoad< Texture2D >( "spear_diffuse" );
	auto spear_normal = Resource::GetOrLoad< Texture2D >( "spear_normal" );
	auto spear_specular = Resource::GetOrLoad< Texture2D >( "spear_specular" );

	ArrayHandle vao;
	Rendering::GenVertexArrays( 1, &vao );
	Rendering::BindVertexArray( vao );

	BufferHandle vbo[ 5 ];
	Rendering::GenBuffers( 5, vbo );

	std::vector< Vector3 > pos_;
	std::vector< Vector2 > tex_;
	std::vector< Vector3 > nor_;

	for ( uint32_t i = 0; i < spear->GetVertexCount(); ++i )
	{
		pos_.push_back( spear->m_Positions[ spear->m_Vertices[ i ][ 1 ] ] );
		tex_.push_back( spear->m_Texels[ spear->m_Vertices[ i ][ 5 ] ] );
		nor_.push_back( spear->m_Normals[ spear->m_Vertices[ i ][ 2 ] ] );
	}

	// Bind positions
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 0 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, pos_.size() * sizeof( pos_[ 0 ] ), pos_.data(), DataUsage::STATIC_DRAW );
	Rendering::VertexAttribPointer( 0, 3, DataType::FLOAT, false, 3 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 0 );

	// Bind texels
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 1 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, tex_.size() * sizeof( tex_[ 0 ] ), tex_.data(), DataUsage::STATIC_DRAW );
	Rendering::VertexAttribPointer( 1, 2, DataType::FLOAT, false, 2 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 1 );

	// Bind normals
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 2 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, nor_.size() * sizeof( nor_[ 0 ] ), nor_.data(), DataUsage::STATIC_DRAW );
	Rendering::VertexAttribPointer( 2, 3, DataType::FLOAT, false, 3 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 2 );

	// Bind tangents
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 2 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, spear->m_Tangents.size() * sizeof( spear->m_Tangents[ 0 ] ), spear->m_Tangents.data(), DataUsage::STATIC_DRAW );
	Rendering::VertexAttribPointer( 3, 3, DataType::FLOAT, false, 3 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 3 );

	// Bind bitangents
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, vbo[ 2 ] );
	Rendering::BufferData( BufferTarget::ARRAY_BUFFER, spear->m_Bitangents.size() * sizeof( spear->m_Bitangents[ 0 ] ), spear->m_Bitangents.data(), DataUsage::STATIC_DRAW );
	Rendering::VertexAttribPointer( 4, 3, DataType::FLOAT, false, 3 * sizeof( float ), ( void* )0 );
	Rendering::EnableVertexAttribArray( 4 );

	//-------------------------------------------------------------
	Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, 0 );
	Rendering::BindVertexArray( 0 );


	ShaderProgramHandle shader = LoadShaders( Shader_Basic_Lit_Vertex, Shader_Basic_Lit_Fragment );
	Rendering::UseProgram( shader );

	auto PVMLocation = Rendering::GetUniformLocation( shader, "PVM" );
	auto PVLocation = Rendering::GetUniformLocation( shader, "PV" );
	auto MLocation = Rendering::GetUniformLocation( shader, "M" );
	auto DiffuseTextureLoc = Rendering::GetUniformLocation( shader, "DiffuseTexture" );
	auto NormalTextureLoc = Rendering::GetUniformLocation( shader, "NormalTexture" );
	auto SpecularTextureLoc = Rendering::GetUniformLocation( shader, "SpecularTexture" );
	auto LightColourLoc = Rendering::GetUniformLocation( shader, "LightColour" );
	auto CameraObjectLoc = Rendering::GetUniformLocation( shader, "CameraObject" );
	auto LightObjectLoc = Rendering::GetUniformLocation( shader, "LightObject" );
	auto AmbientLightLoc = Rendering::GetUniformLocation( shader, "AmbientLight" );

	// Set light colour
	Rendering::Uniform4f( LightColourLoc, 1.0f, 1.0f, 1.0f, 1.0f );
	Rendering::Uniform4f( AmbientLightLoc, 0.3f, 0.3f, 0.3f, 1.0f );

	TextureHandle tbo[ 3 ];
	Rendering::GenTextures( 3, tbo );

	Rendering::ActiveTexture( 0 );
	Rendering::BindTexture( TextureTarget::TEXTURE_2D, tbo[ 0 ] );
	Rendering::TexImage2D( TextureTarget::TEXTURE_2D, 0, TextureFormat( 0 ), spear_diffuse->GetWidth(), spear_diffuse->GetHeight(), 0, TextureFormat( 0 ), TextureSetting( 0 ), spear_diffuse->GetData() );
	Rendering::Uniform1i( DiffuseTextureLoc, 0 );

	Rendering::ActiveTexture( 1 );
	Rendering::BindTexture( TextureTarget::TEXTURE_2D, tbo[ 1 ] );
	Rendering::TexImage2D( TextureTarget::TEXTURE_2D, 0, TextureFormat( 0 ), spear_normal->GetWidth(), spear_normal->GetHeight(), 0, TextureFormat( 0 ), TextureSetting( 0 ), spear_normal->GetData() );
	Rendering::Uniform1i( NormalTextureLoc, 1 );

	Rendering::ActiveTexture( 2 );
	Rendering::BindTexture( TextureTarget::TEXTURE_2D, tbo[ 2 ] );
	Rendering::TexImage2D( TextureTarget::TEXTURE_2D, 0, TextureFormat( 0 ), spear_specular->GetWidth(), spear_specular->GetHeight(), 0, TextureFormat( 0 ), TextureSetting( 0 ), spear_specular->GetData() );
	Rendering::Uniform1i( SpecularTextureLoc, 2 );

	Rendering::ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	Rendering::Enable( RenderSetting::CULL_FACE );
	Rendering::CullFace( CullFaceMode::FRONT );
	//Rendering::Enable( RenderSetting::DEPTH_TEST );
	Rendering::ClearDepth( 1000.0f );


	Vector3 LightObject( 0.0f );
	auto Proj = Matrix4::CreateProjection( Math::Radians( 75.0f ), 1.0f, 0.1f, 100.0f );

	Vector3 CameraPos = Vector3( 0.0f, 2.0f, -5.0f );
	auto View = Matrix4::CreateLookAt( CameraPos, Vector3::Zero, Vector3::Up );
	Rendering::Uniform3f( CameraObjectLoc, CameraPos.x, CameraPos.y, CameraPos.z );
	Matrix4 PV = Math::Multiply( Proj, View );
	float i = 0.0f;

	while ( 1 )
	{
		i += 0.1f;
		auto M = Matrix4::CreateTransform( Vector3( 0.0f, 2.0f * Math::Sin( i ) - 3.0f, 0.0f ), Quaternion::ToQuaternion( Vector3( 0.0f, 0.0f, 0.0f ) ), Vector3::One * 2.0f );
		auto PVM = Math::Multiply( PV, M );
		Rendering::UniformMatrix4fv( PVMLocation, 1, false, &PVM[ 0 ] );
		Rendering::UniformMatrix4fv( PVLocation, 1, false, &PV[ 0 ] );
		Rendering::UniformMatrix4fv( MLocation, 1, false, &M[ 0 ] );

		LightObject.x = Math::Cos( i );
		LightObject.y = 0.0f;
		LightObject.z = Math::Sin( i );
		Rendering::Uniform3f( LightObjectLoc, LightObject.x, LightObject.y, LightObject.z );

		Sleep( 1 );
		Rendering::Clear( BufferFlag::COLOUR_BUFFER_BIT | BufferFlag::DEPTH_BUFFER_BIT );
		Rendering::BindVertexArray( vao );
		Rendering::DrawArrays( RenderMode::TRIANGLE, 0, pos_.size() );
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
	//RunCubeDemo();
	RunLitSpearDemo();
	//RunSquaresDemo();
	//RunOldDemo();
	
	return 0;
}