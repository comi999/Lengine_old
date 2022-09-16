#include "../../Rendering.hpp"
#include "../../ConsoleGL.hpp"
#include "../../Shader.hpp"

uint32_t            ActiveModelTransformLocation = 0;
uint32_t            ActivePVMTransformLocation   = 0;
ShaderProgramHandle ActiveShaderProgram          = 0;


void Rendering::Init()
{
	ConsoleGL::Init();
	ConsoleGL::Enable( ConsoleGL::RenderSetting::CULL_FACE );
	ConsoleGL::Enable( ConsoleGL::RenderSetting::DEPTH_TEST );
	ConsoleGL::CullFace( ConsoleGL::CullFaceMode::BACK );
	ConsoleGL::ClearDepth( 1000.0f );
	ConsoleGL::ClearColour( 0.0, 0.0, 0.0, 1.0f );
}

void Rendering::Clear()
{
	ConsoleGL::Clear( ( uint8_t )ConsoleGL::BufferFlag::DEPTH_BUFFER_BIT );
	ConsoleGL::Clear( ( uint8_t )ConsoleGL::BufferFlag::COLOUR_BUFFER_BIT );
}

void Rendering::CompileProgram( const std::string& a_Source, ShaderProgramHandle& o_ShaderProgramHandle )
{
	if ( a_Source.empty() )
	{
		o_ShaderProgramHandle = 0;
		return;
	}

	// Get the source for vertex shader
	std::string VertexShaderSource = "Shader_";
	std::string FragmentShaderSource = "Shader_";
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

	ShaderHandle VertexShaderID = ConsoleGL::CreateShader( ShaderType::VERTEX_SHADER );
	ShaderHandle FragmentShaderID = ConsoleGL::CreateShader( ShaderType::FRAGMENT_SHADER );

	auto VertexShaderIter = Internal::ShaderFuncLookup::Value.find( CRC32_RT( VertexShaderSource.c_str() ) );
	auto FragmentShaderIter = Internal::ShaderFuncLookup::Value.find( CRC32_RT( FragmentShaderSource.c_str() ) );

	const void* VertexSource = VertexShaderIter != Internal::ShaderFuncLookup::Value.end() ? VertexShaderIter->second : nullptr;
	const void* FragmentSource = FragmentShaderIter != Internal::ShaderFuncLookup::Value.end() ? FragmentShaderIter->second : nullptr;

	ConsoleGL::ShaderSource( VertexShaderID, 1, &VertexSource, nullptr );
	ConsoleGL::CompileShader( VertexShaderID );
	ConsoleGL::ShaderSource( FragmentShaderID, 1, &FragmentSource, nullptr );
	ConsoleGL::CompileShader( FragmentShaderID );

	o_ShaderProgramHandle = ConsoleGL::CreateProgram();
	ConsoleGL::AttachShader( o_ShaderProgramHandle, VertexShaderID );
	ConsoleGL::AttachShader( o_ShaderProgramHandle, FragmentShaderID );
	ConsoleGL::LinkProgram( o_ShaderProgramHandle );

	ConsoleGL::DetachShader( o_ShaderProgramHandle, VertexShaderID );
	ConsoleGL::DetachShader( o_ShaderProgramHandle, FragmentShaderID );
	ConsoleGL::DeleteShader( VertexShaderID );
	ConsoleGL::DeleteShader( FragmentShaderID );

	ConsoleGL::s_ShaderProgramRegistry;
}

void Rendering::DeleteProgram( ShaderProgramHandle a_ShaderProgramHandle )
{
	ConsoleGL::DeleteProgram( a_ShaderProgramHandle );
}

void Rendering::ApplyMesh( const Mesh& a_Mesh )
{
	static ArrayHandle ActiveArrayHandle = 0;

	// First check if an array exists.
	if ( !ActiveArrayHandle )
	{
		ConsoleGL::GenVertexArrays( 1, &ActiveArrayHandle );
	}

	static BufferHandle ActiveBufferHandles[ 6 ] { 0 };

	// First check that there's at least 6 buffer handles available for all mesh attributes.
	for ( uint32_t i = 0; i < 6; ++i )
	{
		if ( !ActiveBufferHandles[ i ] )
		{
			ConsoleGL::GenBuffers( 1, &ActiveBufferHandles[ i ] );
		}
	}

	// Bind the active array handle so all subsequent operations operate on it.
	ConsoleGL::BindVertexArray( ActiveArrayHandle );
	uint32_t VertexCount = a_Mesh.GetVertexCount();

	// Setup all of the attributes.
	if ( a_Mesh.HasPositions() )
	{
		ConsoleGL::BindBuffer( ConsoleGL::BufferTarget::ARRAY_BUFFER, ActiveBufferHandles[ 0 ] );
		ConsoleGL::BufferData( ConsoleGL::BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), a_Mesh.GetPositions(), ConsoleGL::DataUsage::DRAW );
		ConsoleGL::VertexAttribPointer( 0, 3, ConsoleGL::DataType::FLOAT, false, sizeof( Vector3 ), ( void* )0 );
		ConsoleGL::EnableVertexAttribArray( 0 );
	}

	if ( a_Mesh.HasTexels() )
	{
		ConsoleGL::BindBuffer( ConsoleGL::BufferTarget::ARRAY_BUFFER, ActiveBufferHandles[ 1 ] );
		ConsoleGL::BufferData( ConsoleGL::BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector2 ), a_Mesh.GetTexels(), ConsoleGL::DataUsage::DRAW );
		ConsoleGL::VertexAttribPointer( 1, 2, ConsoleGL::DataType::FLOAT, false, sizeof( Vector2 ), ( void* )0 );
		ConsoleGL::EnableVertexAttribArray( 1 );
	}

	if ( a_Mesh.HasColours() )
	{
		ConsoleGL::BindBuffer( ConsoleGL::BufferTarget::ARRAY_BUFFER, ActiveBufferHandles[ 2 ] );
		ConsoleGL::BufferData( ConsoleGL::BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector4 ), a_Mesh.GetColours(), ConsoleGL::DataUsage::DRAW );
		ConsoleGL::VertexAttribPointer( 2, 4, ConsoleGL::DataType::FLOAT, false, sizeof( Vector4 ), ( void* )0 );
		ConsoleGL::EnableVertexAttribArray( 2 );
	}

	if ( a_Mesh.HasNormals() )
	{
		ConsoleGL::BindBuffer( ConsoleGL::BufferTarget::ARRAY_BUFFER, ActiveBufferHandles[ 3 ] );
		ConsoleGL::BufferData( ConsoleGL::BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), a_Mesh.GetNormals(), ConsoleGL::DataUsage::DRAW );
		ConsoleGL::VertexAttribPointer( 3, 3, ConsoleGL::DataType::FLOAT, false, sizeof( Vector3 ), ( void* )0 );
		ConsoleGL::EnableVertexAttribArray( 3 );
	}

	if ( a_Mesh.HasTangents() )
	{
		ConsoleGL::BindBuffer( ConsoleGL::BufferTarget::ARRAY_BUFFER, ActiveBufferHandles[ 4 ] );
		ConsoleGL::BufferData( ConsoleGL::BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), a_Mesh.GetTangents(), ConsoleGL::DataUsage::DRAW );
		ConsoleGL::VertexAttribPointer( 4, 3, ConsoleGL::DataType::FLOAT, false, sizeof( Vector3 ), ( void* )0 );
		ConsoleGL::EnableVertexAttribArray( 4 );
	}

	if ( a_Mesh.HasBitangents() )
	{
		ConsoleGL::BindBuffer( ConsoleGL::BufferTarget::ARRAY_BUFFER, ActiveBufferHandles[ 5 ] );
		ConsoleGL::BufferData( ConsoleGL::BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), a_Mesh.GetBitangents(), ConsoleGL::DataUsage::DRAW );
		ConsoleGL::VertexAttribPointer( 5, 3, ConsoleGL::DataType::FLOAT, false, sizeof( Vector3 ), ( void* )0 );
		ConsoleGL::EnableVertexAttribArray( 5 );
	}

	ConsoleGL::BindVertexArray( 0 );
	ConsoleGL::BindVertexArray( ActiveArrayHandle );
	Rendering::ActiveMesh = &a_Mesh;
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
			ConsoleGL::GenTextures( 1, &ActiveTextureHandles[ CurrentTextureUnit ] );
		}

		ConsoleGL::ActiveTexture( CurrentTextureUnit );
		ConsoleGL::BindTexture( ConsoleGL::TextureTarget::TEXTURE_2D, ActiveTextureHandles[ CurrentTextureUnit ] );
		ConsoleGL::TexImage2D( ConsoleGL::TextureTarget::TEXTURE_2D, 0, ConsoleGL::TextureFormat( 0 ), Begin->second.GetTexture()->GetWidth(), Begin->second.GetTexture()->GetHeight(), 0, ConsoleGL::TextureFormat( 0 ), ConsoleGL::TextureSetting( 0 ), Begin->second.GetTexture()->GetData() );
		Rendering::ApplyUniform( Begin->second.GetName().Data(), 1, &CurrentTextureUnit );
		++CurrentTextureUnit;
	}
}

void Rendering::ApplyShader( const Shader& a_Shader )
{
	if ( !a_Shader.GetHandle() )
	{
		CompileProgram( a_Shader.GetSource(), const_cast< Shader& >( a_Shader ).GetHandle() );
	}

	ConsoleGL::UseProgram( a_Shader.GetHandle() );
	ActiveShaderProgram = a_Shader.GetHandle();
}

void Rendering::ApplyUniform( const char* a_Name, uint32_t a_Count, const float* a_Value )
{
	if ( !ActiveShaderProgram )
	{
		return;
	}

	auto UniformLocation = ConsoleGL::GetUniformLocation( ActiveShaderProgram, a_Name );

	if ( UniformLocation > -1 )
	{
		ConsoleGL::Uniform1fv( UniformLocation, a_Count, a_Value );
	}
}

void Rendering::ApplyUniform( const char* a_Name, uint32_t a_Count, const int32_t* a_Value )
{
	if ( !ActiveShaderProgram )
	{
		return;
	}

	auto UniformLocation = ConsoleGL::GetUniformLocation( ActiveShaderProgram, a_Name );

	if ( UniformLocation > -1 )
	{
		ConsoleGL::Uniform1iv( UniformLocation, a_Count, a_Value );
	}
}

void Rendering::ApplyUniform( const char* a_Name, uint32_t a_Count, const uint32_t* a_Value )
{
	if ( !ActiveShaderProgram )
	{
		return;
	}

	auto UniformLocation = ConsoleGL::GetUniformLocation( ActiveShaderProgram, a_Name );

	if ( UniformLocation > -1 )
	{
		ConsoleGL::Uniform1uiv( UniformLocation, a_Count, a_Value );
	}
}

void Rendering::ApplyUniform( const char* a_Name, uint32_t a_Count, const Matrix4* a_Value )
{
	if ( !ActiveShaderProgram )
	{
		return;
	}

	auto UniformLocation = ConsoleGL::GetUniformLocation( ActiveShaderProgram, a_Name );

	if ( UniformLocation > -1 )
	{
		ConsoleGL::UniformMatrix4fv( UniformLocation, a_Count, false, a_Value->Data );
	}
}

void Rendering::Draw()
{
	ConsoleGL::DrawElements( ConsoleGL::RenderMode::TRIANGLE, Rendering::ActiveMesh->GetIndexCount(), ConsoleGL::DataType::UNSIGNED_INT, Rendering::ActiveMesh->GetIndices() );
}