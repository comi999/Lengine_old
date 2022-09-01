
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../../Rendering.hpp"
#include "../../Shader.hpp"

uint32_t            ActiveModelTransformLocation = 0;
uint32_t            ActivePVMTransformLocation   = 0;
ShaderProgramHandle ActiveShaderProgram          = 0;
const Mesh*         ActiveMesh                   = nullptr;
ArrayHandle         ActiveArrayHandle = 0;

GLFWwindow* Window;

void Rendering::Init()
{
	auto glfwStatus = glfwInit();
	glewExperimental = GL_TRUE;
	
	Window = glfwCreateWindow( 1000, 1000, "Test title", 0, 0 );
	glfwMakeContextCurrent( Window );
	
	/*glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );*/
	
	bool glewStatus = glewInit() == GLEW_OK;
	

	auto version = glfwGetVersionString();
	
	//glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	//glCullFace( GL_BACK );
	//glClearDepth( 0000.0f );
	glClearColor( 0.0, 0.0, 0.0, 1.0f );
}

void Rendering::Clear()
{
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Rendering::CompileProgram( const std::string& a_Source, ShaderProgramHandle& o_ShaderProgramHandle )
{
	if ( a_Source.empty() )
	{
		o_ShaderProgramHandle = 0;
		return;
	}

	// Get the source for vertex shader
	std::string VertexShaderSource = "";
	std::string FragmentShaderSource = "";
	// Do stuff to get out the name

	auto VertexOffset   = a_Source.find( "VERTEX:"   );
	auto FragmentOffset = a_Source.find( "FRAGMENT:" );

	if ( VertexOffset == std::string::npos || FragmentOffset == std::string::npos )
	{
		o_ShaderProgramHandle = 0;
		return;
	}

	{
		std::string_view VertexSourceView( a_Source.c_str(), FragmentOffset );
		VertexSourceView = VertexSourceView.substr( VertexOffset + 7, VertexSourceView.size() - 7 );
		auto Padding = VertexSourceView.find_first_not_of( " \n" );
		VertexSourceView = VertexSourceView.substr( Padding, VertexSourceView.size() - Padding );
		Padding = VertexSourceView.find_last_not_of( " \n" );
		VertexSourceView = VertexSourceView.substr( 0, Padding + 1 );
		VertexShaderSource += VertexSourceView;
	}

	{
		std::string_view FragmentSourceView( a_Source.c_str() + FragmentOffset, a_Source.size() - FragmentOffset );
		FragmentSourceView = FragmentSourceView.substr( 9, FragmentSourceView.size() - 9 );
		auto Padding = FragmentSourceView.find_first_not_of( " \n" );
		FragmentSourceView = FragmentSourceView.substr( Padding, FragmentSourceView.size() - Padding );
		Padding = FragmentSourceView.find_last_not_of( " \n" );
		FragmentSourceView = FragmentSourceView.substr( 0, Padding + 1 );
		FragmentShaderSource += FragmentSourceView;
	}

	auto VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
	auto FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );
	
	auto VertexSourcePtr = VertexShaderSource.c_str();
	auto FragmentSourcePtr = FragmentShaderSource.c_str();

	glShaderSource( VertexShaderID, 1, &VertexSourcePtr, nullptr );
	glShaderSource( FragmentShaderID, 1, &FragmentSourcePtr, nullptr );

	GLsizei siz;
	char buff[ 100 ];

	glCompileShader( VertexShaderID );
	glGetShaderInfoLog( VertexShaderID, 100, &siz, buff );

	glCompileShader( FragmentShaderID );
	glGetShaderInfoLog( FragmentShaderID, 100, &siz, buff );

	o_ShaderProgramHandle = glCreateProgram();
	glAttachShader( o_ShaderProgramHandle, VertexShaderID );
	glAttachShader( o_ShaderProgramHandle, FragmentShaderID );
	glLinkProgram( o_ShaderProgramHandle );

	glGetProgramInfoLog( o_ShaderProgramHandle, 100, &siz, buff );

	glDetachShader( o_ShaderProgramHandle, VertexShaderID );
	glDetachShader( o_ShaderProgramHandle, FragmentShaderID );
	glDeleteShader( VertexShaderID );
	glDeleteShader( FragmentShaderID );

	/*ShaderHandle VertexShaderID = glCreateShader( ShaderType::VERTEX_SHADER );
	ShaderHandle FragmentShaderID = glCreateShader( ShaderType::FRAGMENT_SHADER );

	auto VertexShaderIter = Internal::ShaderFuncLookup::Value.find( CRC32_RT( VertexShaderSource.c_str() ) );
	auto FragmentShaderIter = Internal::ShaderFuncLookup::Value.find( CRC32_RT( FragmentShaderSource.c_str() ) );

	const void* VertexSource = VertexShaderIter != Internal::ShaderFuncLookup::Value.end() ? VertexShaderIter->second : nullptr;
	const void* FragmentSource = FragmentShaderIter != Internal::ShaderFuncLookup::Value.end() ? FragmentShaderIter->second : nullptr;

	glShaderSource( VertexShaderID, 1, &VertexSource, nullptr );
	glCompileShader( VertexShaderID );
	glShaderSource( FragmentShaderID, 1, &FragmentSource, nullptr );
	glCompileShader( FragmentShaderID );

	o_ShaderProgramHandle = glCreateProgram();
	glAttachShader( o_ShaderProgramHandle, VertexShaderID );
	glAttachShader( o_ShaderProgramHandle, FragmentShaderID );
	glLinkProgram( o_ShaderProgramHandle );

	glDetachShader( o_ShaderProgramHandle, VertexShaderID );
	glDetachShader( o_ShaderProgramHandle, FragmentShaderID );
	glDeleteShader( VertexShaderID );
	glDeleteShader( FragmentShaderID );

	gls_ShaderProgramRegistry;*/
}

void Rendering::DeleteProgram( ShaderProgramHandle a_ShaderProgramHandle )
{
	glDeleteProgram( a_ShaderProgramHandle );
}

void Rendering::ApplyMesh( const Mesh& a_Mesh )
{
	// First check if an array exists.
	if ( !ActiveArrayHandle )
	{
		glGenVertexArrays( 1, &ActiveArrayHandle );
	}

	static BufferHandle ActiveBufferHandles[ 6 ] { 0 };

	// First check that there's at least 6 buffer handles available for all mesh attributes.
	for ( uint32_t i = 0; i < 6; ++i )
	{
		if ( !ActiveBufferHandles[ i ] )
		{
			glGenBuffers( 1, &ActiveBufferHandles[ i ] );
		}
	}

	// Bind the active array handle so all subsequent operations operate on it.
	glBindVertexArray( ActiveArrayHandle );
	uint32_t VertexCount = a_Mesh.GetVertexCount();

	// Setup all of the attributes.
	if ( a_Mesh.HasPositions() )
	{
		glBindBuffer( GL_ARRAY_BUFFER, ActiveBufferHandles[ 0 ] );

		float verts[]
		{
			1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
		};

		glBufferData( GL_ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), a_Mesh.GetPositions(), GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( Vector3 ), ( void* )0 );
		glEnableVertexAttribArray( 0 );
	}

	/*if ( a_Mesh.HasTexels() )
	{
		glBindBuffer( GL_ARRAY_BUFFER, ActiveBufferHandles[ 1 ] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount * sizeof( Vector2 ), a_Mesh.GetTexels(), GL_STATIC_DRAW );
		glVertexAttribPointer( 1, 2, GL_FLOAT, false, sizeof( Vector2 ), ( void* )0 );
		glEnableVertexAttribArray( 1 );
	}

	if ( a_Mesh.HasColours() )
	{
		glBindBuffer( GL_ARRAY_BUFFER, ActiveBufferHandles[ 2 ] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount * sizeof( Vector4 ), a_Mesh.GetColours(), GL_STATIC_DRAW );
		glVertexAttribPointer( 2, 4, GL_FLOAT, false, sizeof( Vector4 ), ( void* )0 );
		glEnableVertexAttribArray( 2 );
	}

	if ( a_Mesh.HasNormals() )
	{
		glBindBuffer( GL_ARRAY_BUFFER, ActiveBufferHandles[ 3 ] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), a_Mesh.GetNormals(), GL_STATIC_DRAW );
		glVertexAttribPointer( 3, 3, GL_FLOAT, false, sizeof( Vector3 ), ( void* )0 );
		glEnableVertexAttribArray( 3 );
	}

	if ( a_Mesh.HasTangents() )
	{
		glBindBuffer( GL_ARRAY_BUFFER, ActiveBufferHandles[ 4 ] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), a_Mesh.GetTangents(), GL_STATIC_DRAW );
		glVertexAttribPointer( 4, 3, GL_FLOAT, false, sizeof( Vector3 ), ( void* )0 );
		glEnableVertexAttribArray( 4 );
	}

	if ( a_Mesh.HasBitangents() )
	{
		glBindBuffer( GL_ARRAY_BUFFER, ActiveBufferHandles[ 5 ] );
		glBufferData( GL_ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), a_Mesh.GetBitangents(), GL_STATIC_DRAW );
		glVertexAttribPointer( 5, 3, GL_FLOAT, false, sizeof( Vector3 ), ( void* )0 );
		glEnableVertexAttribArray( 5 );
	}*/

	glBindVertexArray( 0 );
	ActiveMesh = &a_Mesh;
}

void Rendering::ApplyMaterial( const Material& a_Material )
{
	ApplyShader( a_Material.GetShader() );

	for ( auto Begin = a_Material.GetPropertyBegin(), End = a_Material.GetPropertyEnd(); Begin != End; ++Begin )
	{
		auto Name = Begin->second.GetName().Data();
		auto Count = Begin->second.GetSize();

		switch ( Begin->second.GetType() )
		{
			case MaterialProperty::Type::INT:
			{
				Vector4Int Value;
				Begin->second.Get( Value );
				Rendering::ApplyUniform( Name, Count, &Value[ 0 ] );
			}
			case MaterialProperty::Type::FLOAT:
			{
				Vector4 Value;
				Begin->second.Get( Value );
				Rendering::ApplyUniform( Name, Count, &Value[ 0 ] );
			}
		}
	}

	static TextureHandle ActiveTextureHandles[ 6 ];

	int CurrentTextureUnit = 0;

	for ( auto Begin = a_Material.GetTextureBegin(), End = a_Material.GetTextureEnd(); Begin != End; ++Begin )
	{
		if ( !ActiveTextureHandles[ CurrentTextureUnit ] )
		{
			glGenTextures( 1, &ActiveTextureHandles[ CurrentTextureUnit ] );
		}

		glActiveTexture( GL_TEXTURE0 + CurrentTextureUnit );
		glBindTexture( GL_TEXTURE_2D, ActiveTextureHandles[ CurrentTextureUnit ] );
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Begin->second.GetTexture()->GetWidth(), Begin->second.GetTexture()->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Begin->second.GetTexture()->GetData() );
		Rendering::ApplyUniform( Begin->second.GetName().Data(), 1, &ActiveTextureHandles[ CurrentTextureUnit ] );
		++CurrentTextureUnit;
	}


	//	static int ActiveTextureUnit = 0;

	//	for ( auto
	//		  Begin = const_cast< Material* >( this )->m_Textures.begin(),
	//		  End = const_cast< Material* >( this )->m_Textures.end();
	//		  Begin != End; ++Begin )
	//	{
	//		if ( Begin->second.m_Location < 0 )
	//		{
	//			continue;
	//		}

	//		if ( !Begin->second.m_Handle )
	//		{


	//			Rendering::GenTextures( 1, &Begin->second.m_Handle );
	//			Rendering::ActiveTexture( ActiveTextureUnit );
	//			Rendering::BindTexture( TextureTarget::TEXTURE_2D, Begin->second.m_Handle );
	//			Rendering::TexImage2D( TextureTarget::TEXTURE_2D, 0, TextureFormat( 0 ), Begin->second.m_Resource.Assure()->GetWidth(), Begin->second.m_Resource.Assure()->GetHeight(), 0, TextureFormat( 0 ), TextureSetting( 0 ), Begin->second.m_Resource.Assure()->GetData() );
	//			Begin->second.m_TextureUnit = ActiveTextureUnit++;
	//		}

	//		Rendering::Uniform1i( Begin->second.m_Location, Begin->second.m_TextureUnit );
	//	}

	// There should be an unapply function too to free all texture handles etc.

	/*void FindLocations()
	{
		for ( auto Begin = m_Attributes.begin(), End = m_Attributes.end(); Begin != End; ++Begin )
		{
			Begin->second.m_Location = Rendering::GetUniformLocation( m_Shader->GetProgramHandle(), Begin->second.GetName().Data() );
		}

		for ( auto Begin = m_Textures.begin(), End = m_Textures.end(); Begin != End; ++Begin )
		{
			Begin->second.m_Location = Rendering::GetUniformLocation( m_Shader->GetProgramHandle(), Begin->second.GetName().Data() );
		}
	}*/
}

#include <sstream>

GLuint LoadShadersImpl( const char* vertex_file_path, const char* fragment_file_path )
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

void Rendering::ApplyShader( const Shader& a_Shader )
{
	static GLuint shader = LoadShadersImpl( "shader.vert", "shader.frag" );
	glUseProgram( shader );
	ActiveShaderProgram = shader;
	return;

	if ( !a_Shader.GetHandle() )
	{
		CompileProgram( a_Shader.GetSource(), const_cast< Shader& >( a_Shader ).GetHandle() );
	}

	glUseProgram( a_Shader.GetHandle() );
	ActiveShaderProgram = a_Shader.GetHandle();
}

void Rendering::ApplyUniform( const char* a_Name, uint32_t a_Count, const float* a_Value )
{
	if ( !ActiveShaderProgram )
	{
		return;
	}

	auto UniformLocation = glGetUniformLocation( ActiveShaderProgram, a_Name );

	if ( UniformLocation > -1 )
	{
		glUniform1fv( UniformLocation, a_Count, a_Value );
	}
}

void Rendering::ApplyUniform( const char* a_Name, uint32_t a_Count, const int32_t* a_Value )
{
	if ( !ActiveShaderProgram )
	{
		return;
	}

	auto UniformLocation = glGetUniformLocation( ActiveShaderProgram, a_Name );

	if ( UniformLocation > -1 )
	{
		glUniform1iv( UniformLocation, a_Count, a_Value );
	}
}

void Rendering::ApplyUniform( const char* a_Name, uint32_t a_Count, const uint32_t* a_Value )
{
	if ( !ActiveShaderProgram )
	{
		return;
	}

	auto UniformLocation = glGetUniformLocation( ActiveShaderProgram, a_Name );

	if ( UniformLocation > -1 )
	{
		glUniform1uiv( UniformLocation, a_Count, a_Value );
	}
}

void Rendering::ApplyUniform( const char* a_Name, uint32_t a_Count, const Matrix4* a_Value )
{
	if ( !ActiveShaderProgram )
	{
		return;
	}

	auto UniformLocation = glGetUniformLocation( ActiveShaderProgram, a_Name );

	if ( UniformLocation > -1 )
	{
		glUniformMatrix4fv( UniformLocation, a_Count, false, a_Value->Data );
	}
}

void Rendering::Draw()
{
	//glUseProgram( ActiveShaderProgram );
	glBindVertexArray( ActiveArrayHandle );
	glDrawElements( GL_TRIANGLES, ActiveMesh->GetIndexCount(), GL_UNSIGNED_INT, ActiveMesh->GetIndices() );
	glBindVertexArray( 0 );
	glfwSwapBuffers( Window );
	glfwWindowShouldClose( Window );
	glfwPollEvents();
}