#include "Rendering.hpp"
#include "ConsoleWindow.hpp"

void Rendering::Init()
{
	s_DepthBuffer.Init( ConsoleWindow::GetCurrentContext()->GetSize() );
}

void Rendering::GenBuffers( uint32_t a_Count, BufferHandle* a_Handles )
{
	while ( a_Count-- > 0 ) a_Handles[ a_Count ] = s_BufferRegistry.Create();
}

void Rendering::BindBuffer( BufferTarget a_BufferTarget, BufferHandle a_Handle )
{
	s_BufferTargets[ ( uint32_t )a_BufferTarget ] = a_Handle;
}

void Rendering::DeleteBuffers( uint32_t a_Count, BufferHandle* a_Handles )
{
	while ( a_Count-- > 0 )
	{
		// Unbind from buffer target if exists.
		for ( size_t i = 0; i < 14; ++i )
		{
			if ( s_BufferTargets[ i ] == a_Handles[ a_Count ] )
			{
				s_BufferTargets[ i ] = 0;
				break;
			}
		}

		// Destroy buffer.
		s_BufferRegistry.Destroy( a_Handles[ a_Count ] );
	}
}

bool Rendering::IsBuffer( BufferHandle a_Handle )
{
	return s_BufferRegistry.Valid( a_Handle );
}

void Rendering::UseProgram( ShaderProgramHandle a_ShaderProgramHandle )
{
	s_ActiveShaderProgram = a_ShaderProgramHandle;
}

void Rendering::Clear( uint8_t a_Flags )
{
	if ( a_Flags & static_cast< uint8_t >( BufferFlag::COLOUR_BUFFER_BIT ) )
	{
		ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetBuffer( s_ClearColour );
	}

	if ( a_Flags & static_cast< uint8_t >( BufferFlag::DEPTH_BUFFER_BIT ) )
	{
		s_DepthBuffer.Reset( s_ClearDepth );
	}

	if ( a_Flags & static_cast< uint8_t >( BufferFlag::ACCUM_BUFFER_BIT ) )
	{

	}

	if ( a_Flags & static_cast< uint8_t >( BufferFlag::STENCIL_BUFFER_BIT ) )
	{

	}
}

void Rendering::ClearColour( float a_R, float a_G, float a_B, float a_A )
{
	s_ClearColour = PixelColourMap::Get().ConvertColour( {
		static_cast< unsigned char >( 255u * a_R ),
		static_cast< unsigned char >( 255u * a_G ),
		static_cast< unsigned char >( 255u * a_B ),
		static_cast< unsigned char >( 255u * a_A ) } );
}

void Rendering::ClearDepth( float a_ClearDepth )
{
	s_ClearDepth = a_ClearDepth;
}

void Rendering::DrawArrays( RenderMode a_Mode, uint32_t a_Begin, uint32_t a_Count )
{
	switch ( a_Mode )
	{
		case RenderMode::POINT:
			break;
		case RenderMode::LINE:
			break;
		case RenderMode::TRIANGLE:
		{
			DrawArraysImpl< 0 >( a_Begin, a_Count );
			break;
		}
		default:
			break;
	}
}

void Rendering::BufferData( BufferTarget a_BufferTarget, size_t a_Size, const void* a_Data, DataUsage a_DataUsage )
{
	Buffer& TargetBuffer = s_BufferRegistry[ s_BufferTargets[ ( uint32_t )a_BufferTarget ] ];
	TargetBuffer.resize( a_Size );
	
	if ( a_Data )
	{
		memcpy( TargetBuffer.data(), a_Data, a_Size );
	}
}

void Rendering::NamedBufferData( BufferHandle a_Handle, size_t a_Size, const void* a_Data, DataUsage a_DataUsage )
{
	Buffer& TargetBuffer = s_BufferRegistry[ a_Handle ];
	TargetBuffer.resize( a_Size );

	if ( a_Data )
	{
		memcpy( TargetBuffer.data(), a_Data, a_Size );
	}
}

void Rendering::GenVertexArrays( uint32_t a_Count, ArrayHandle* a_Handles )
{
	while ( a_Count-- > 0 ) a_Handles[ a_Count ] = s_ArrayRegistry.Create();
}

void Rendering::BindVertexArray( ArrayHandle a_Handle )
{
	if ( !a_Handle )
	{
		s_ActiveArray = 0;
		return;
	}

	s_ActiveArray = a_Handle;
	auto& ActiveArray = s_ArrayRegistry[ a_Handle ];
	
	for ( uint8_t i = 0; i < 8; ++i )
	{
		if ( ActiveArray[ i ].Enabled )
		{
			s_AttributeRegistry[ i ] = ActiveArray[ i ];
		}
	}
}

void Rendering::DeleteVertexArrays( uint32_t a_Count, ArrayHandle* a_Handles )
{
	while ( a_Count-- > 0 )
	{
		// Unbind bound vertex array if exists.
		if ( s_ActiveArray == a_Handles[ a_Count ] )
		{
			s_ActiveArray = 0;
		}

		// Destroy buffer.
		s_ArrayRegistry.Destroy( a_Handles[ a_Count ] );
	}
}

void Rendering::EnableVertexAttribArray( uint32_t a_Position )
{
	s_ArrayRegistry[ s_ActiveArray ][ a_Position ].Enabled = true;
}

void Rendering::DisableVertexAttribArray( uint32_t a_Position )
{
	s_ArrayRegistry[ s_ActiveArray ][ a_Position ].Enabled = false;
}

void Rendering::VertexAttribPointer( uint32_t a_Index, uint32_t a_Size, DataType a_DataType, bool a_Normalized, size_t a_Stride, void* a_Offset )
{
	auto& Attributes = s_ArrayRegistry[ s_ActiveArray ][ a_Index ];
	Attributes.Buffer = s_BufferTargets[ ( uint32_t )BufferTarget::ARRAY_BUFFER ];
	Attributes.Size = a_Size - 1;
	Attributes.Normalized = a_Normalized;
	Attributes.Type = ( uint32_t )a_DataType;
	Attributes.Stride = a_Stride;
	Attributes.Enabled = false;
	Attributes.Offset = ( uint32_t )a_Offset;
}

void Rendering::Enable( RenderSetting a_RenderSetting )
{
	switch ( a_RenderSetting )
	{
		case RenderSetting::DEPTH_TEST:
		{
			s_RenderState.DepthTest = true;
			break;
		}
		case RenderSetting::CULL_FACE:
		{
			s_RenderState.CullFace = true;
			break;
		}
		default:
			break;
	}
}

void Rendering::Disable( RenderSetting a_RenderSetting )
{
	switch ( a_RenderSetting )
	{
		case RenderSetting::DEPTH_TEST:
		{
			s_RenderState.DepthTest = false;
			break;
		}
		case RenderSetting::CULL_FACE:
		{
			s_RenderState.CullFace = false;
			break;
		}
		default:
			break;
	}
}

void Rendering::CullFace( CullFaceMode a_CullFaceMode )
{
	switch ( a_CullFaceMode )
	{
		case CullFaceMode::FRONT:
		{
			s_RenderState.FrontCull = true;
			s_RenderState.BackCull = false;
			break;
		}
		case CullFaceMode::BACK:
		{
			s_RenderState.FrontCull = false;
			s_RenderState.BackCull = true;
			break;
		}
		case CullFaceMode::FRONT_AND_BACK:
		{
			s_RenderState.FrontCull = true;
			s_RenderState.BackCull = true;
			break;
		}
		default:
			break;
	}
}

int32_t Rendering::GetUniformLocation( ShaderProgramHandle a_ShaderProgramHandle, const char* a_Name )
{
	auto& ShaderProgram = s_ShaderProgramRegistry[ a_ShaderProgramHandle ];
	auto UniformName = CRC32_RT( a_Name );
	auto LocationEntry = ShaderProgram.m_UniformLocations.find( UniformName );
	return LocationEntry == ShaderProgram.m_UniformLocations.end() ? -1 : LocationEntry->second;
}

void Rendering::Uniform1f( int32_t a_Location, float a_V0 )
{
	*reinterpret_cast< float* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0 };
}

void Rendering::Uniform2f( int32_t a_Location, float a_V0, float a_V1 )
{
	*reinterpret_cast< Vector2* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1 };
}

void Rendering::Uniform3f( int32_t a_Location, float a_V0, float a_V1, float a_V2 )
{
	*reinterpret_cast< Vector3* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1, a_V2 };
}

void Rendering::Uniform4f( int32_t a_Location, float a_V0, float a_V1, float a_V2, float a_V3 )
{
	*reinterpret_cast< Vector4* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1, a_V2, a_V3 };
}

void Rendering::Uniform1i( int32_t a_Location, int32_t a_V0 )
{
	*reinterpret_cast< int32_t* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0 };
}

void Rendering::Uniform2i( int32_t a_Location, int32_t a_V0, int32_t a_V1 )
{
	*reinterpret_cast< Vector2Int* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1 };
}

void Rendering::Uniform3i( int32_t a_Location, int32_t a_V0, int32_t a_V1, int32_t a_V2 )
{
	*reinterpret_cast< Vector3Int* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1, a_V2 };
}

void Rendering::Uniform4i( int32_t a_Location, int32_t a_V0, int32_t a_V1, int32_t a_V2, int32_t a_V3 )
{
	*reinterpret_cast< Vector4Int* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1, a_V2, a_V3 };
}

void Rendering::Uniform1ui( int32_t a_Location, uint32_t a_V0 )
{
	*reinterpret_cast< uint32_t* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0 };
}

void Rendering::Uniform2ui( int32_t a_Location, uint32_t a_V0, uint32_t a_V1 )
{
	*reinterpret_cast< Vector2UInt* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1 };
}

void Rendering::Uniform3ui( int32_t a_Location, uint32_t a_V0, uint32_t a_V1, uint32_t a_V2 )
{
	*reinterpret_cast< Vector3UInt* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1, a_V2 };
}

void Rendering::Uniform4ui( int32_t a_Location, uint32_t a_V0, uint32_t a_V1, uint32_t a_V2, uint32_t a_V3 )
{
	*reinterpret_cast< Vector4UInt* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = { a_V0, a_V1, a_V2, a_V3 };
}

void Rendering::Uniform1fv( int32_t a_Location, uint32_t a_Count, float* a_Value )
{
	typedef std::array< float, 1 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) = 
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform2fv( int32_t a_Location, uint32_t a_Count, float* a_Value )
{
	typedef std::array< float, 2 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform3fv( int32_t a_Location, uint32_t a_Count, float* a_Value )
{
	typedef std::array< float, 3 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform4fv( int32_t a_Location, uint32_t a_Count, float* a_Value )
{
	typedef std::array< float, 4 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform1iv( int32_t a_Location, uint32_t a_Count, int32_t* a_Value )
{
	typedef std::array< int32_t, 1 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform2iv( int32_t a_Location, uint32_t a_Count, int32_t* a_Value )
{
	typedef std::array< int32_t, 2 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform3iv( int32_t a_Location, uint32_t a_Count, int32_t* a_Value )
{
	typedef std::array< int32_t, 3 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform4iv( int32_t a_Location, uint32_t a_Count, int32_t* a_Value )
{
	typedef std::array< int32_t, 4 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform1uiv( int32_t a_Location, uint32_t a_Count, uint32_t* a_Value )
{
	typedef std::array< uint32_t, 1 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform2uiv( int32_t a_Location, uint32_t a_Count, uint32_t* a_Value )
{
	typedef std::array< uint32_t, 2 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform3uiv( int32_t a_Location, uint32_t a_Count, uint32_t* a_Value )
{
	typedef std::array< uint32_t, 3 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::Uniform4uiv( int32_t a_Location, uint32_t a_Count, uint32_t* a_Value )
{
	typedef std::array< uint32_t, 4 > Type;
	*reinterpret_cast< Type* >( s_ShaderProgramRegistry[ s_ActiveShaderProgram ].m_Uniforms[ a_Location ] ) =
		*reinterpret_cast< Type* >( a_Value );
}

void Rendering::UniformMatrix2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Matrix2* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Matrix2* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Matrix2* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
}

void Rendering::UniformMatrix3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Matrix3* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Matrix3* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Matrix3* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
}

void Rendering::UniformMatrix4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Matrix4* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Matrix4* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Matrix4* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
}

void Rendering::UniformMatrix2x3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	typedef Matrix2x3 Type;
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Type* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Type* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Type* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = reinterpret_cast< Type& >( Value );
		}
	}
}

void Rendering::UniformMatrix3x2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	typedef Matrix3x2 Type;
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Type* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Type* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Type* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = reinterpret_cast< Type& >( Value );
		}
	}
}

void Rendering::UniformMatrix2x4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	typedef Matrix2x4 Type;
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Type* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Type* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Type* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = reinterpret_cast< Type& >( Value );
		}
	}
}

void Rendering::UniformMatrix4x2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	typedef Matrix4x2 Type;
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Type* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Type* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Type* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = reinterpret_cast< Type& >( Value );
		}
	}
}

void Rendering::UniformMatrix3x4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	typedef Matrix3x4 Type;
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Type* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Type* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Type* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = reinterpret_cast< Type& >( Value );
		}
	}
}

void Rendering::UniformMatrix4x3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value )
{
	typedef Matrix4x3 Type;
	auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
	auto* UniformValue = reinterpret_cast< Type* >( ActiveProgram.m_Uniforms[ a_Location ] );

	if ( !a_Transpose )
	{
		auto& Value = *reinterpret_cast< Type* >( a_Value );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = Value;
		}
	}
	else
	{
		auto Value = Math::Transpose( *reinterpret_cast< Type* >( a_Value ) );

		for ( uint32_t i = 0; i < a_Count; ++i )
		{
			UniformValue[ i ] = reinterpret_cast< Type& >( Value );
		}
	}
}

ShaderHandle Rendering::CreateShader( ShaderType a_ShaderType )
{
	ShaderHandle NewHandle = s_ShaderRegistry.Create();
	s_ShaderRegistry[ NewHandle ].Type = a_ShaderType;
	return NewHandle;
}

void Rendering::ShaderSource( ShaderHandle a_ShaderHandle, uint32_t a_Count, const void** a_Sources, uint32_t* a_Lengths )
{
	if ( a_Count < 1 )
	{
		return;
	}

	s_ShaderRegistry[ a_ShaderHandle ].Callback = ( void(*)() )a_Sources[ 0 ];
}

void Rendering::CompileShader( ShaderHandle a_ShaderHandle )
{
	Shader& ActiveShader = s_ShaderRegistry[ a_ShaderHandle ];

	if ( !ActiveShader.Callback )
	{
		ActiveShader.Callback = Shader::Empty;
	}
}

void Rendering::GetShaderIV( ShaderHandle a_ShaderHandle, ShaderInfo a_ShaderInfo, void* a_Value )
{
	switch ( a_ShaderInfo )
	{
		case ShaderInfo::COMPILE_STATUS:
		{
			break;
		}
		case ShaderInfo::INFO_LOG_LENGTH:
		{
			break;
		}
	}
}

void Rendering::GetShaderInfoLog( ShaderHandle a_ShaderHandle, size_t a_BufferSize, size_t* a_Length, char* a_InfoLog )
{}

ShaderProgramHandle Rendering::CreateProgram()
{
	return s_ShaderProgramRegistry.Create();
}

void Rendering::AttachShader( ShaderProgramHandle a_ShaderProgramHandle, ShaderHandle a_ShaderHandle )
{
	auto& Program = s_ShaderProgramRegistry[ a_ShaderProgramHandle ];
	auto& Shader  = s_ShaderRegistry[ a_ShaderHandle ];
	Program.m_Shaders[ ( uint32_t )Shader.Type ].Handle = a_ShaderHandle;
	Program.m_Shaders[ ( uint32_t )Shader.Type ].Set = true;
}

void Rendering::LinkProgram( ShaderProgramHandle a_ShaderProgramHandle )
{
	auto& Program = s_ShaderProgramRegistry[ a_ShaderProgramHandle ];
	
	for ( uint32_t i = 0; i < 2; ++i )
	{
		auto& Entry = Program.m_Shaders[ i ];

		if ( !Entry.Set )
		{
			Entry.Callback = Shader::Empty;
			continue;
		}

		auto Callback = s_ShaderRegistry[ Entry.Handle ].Callback;
		Program.m_Shaders[ i ].Callback = Callback;

		// Get the vector of uniforms registered to the given function.
		auto& Uniforms = s_UniformMap[ Callback ];
		
		for ( auto& Pair : Uniforms )
		{
			// First check if that uniform is already a part of the program.
			if ( Program.m_UniformLocations.find( Pair.first ) == Program.m_UniformLocations.end() )
			{
				Program.m_UniformLocations[ Pair.first ] = Program.m_Uniforms.size();
				Program.m_Uniforms.push_back( Pair.second );
			}
		}
	}

	// Populate uniforms.
	
}

void Rendering::GetProgramIV( ShaderProgramHandle a_ShaderProgramHandle, ShaderInfo a_ShaderInfo, void* a_Value )
{}

void Rendering::GetProgramInfoLog( ShaderProgramHandle a_ShaderProgramHandle, size_t a_BufferSize, size_t * a_Length, char* a_InfoLog )
{}

void Rendering::DetachShader( ShaderProgramHandle a_ShaderProgramHandle, ShaderHandle a_ShaderHandle )
{
	auto& Program = s_ShaderProgramRegistry[ a_ShaderProgramHandle ];
	auto& Shader  = s_ShaderRegistry[ a_ShaderHandle ];
	auto& Entry   = Program.m_Shaders[ ( uint32_t )Shader.Type ];
	Entry.Handle = 0;
	Entry.Set = false;
}

void Rendering::DeleteShader( ShaderHandle a_ShaderHandle )
{
	s_ShaderRegistry.Destroy( a_ShaderHandle );
}

void Rendering::DeleteProgram( ShaderProgramHandle a_ShaderProgramHandle )
{
	s_ShaderProgramRegistry.Destroy( a_ShaderProgramHandle );
}

void Rendering::ActiveTexture( uint32_t a_ActiveTexture )
{
	s_ActiveTextureUnit = a_ActiveTexture;
}

void Rendering::TexParameterf( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, float a_Value )
{
	TexParameterImpl( a_TextureTarget, a_TextureParameter, a_Value );
}

void Rendering::TexParameterfv( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, const float* a_Value )
{
	TexParameterImpl( a_TextureTarget, a_TextureParameter, a_Value );
}

void Rendering::TexParameteri( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, int32_t a_Value )
{
	TexParameterImpl( a_TextureTarget, a_TextureParameter, a_Value );
}

void Rendering::TexParameteri( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, const int32_t* a_Value )
{
	TexParameterImpl( a_TextureTarget, a_TextureParameter, a_Value );
}

void Rendering::TexParameterui( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, uint32_t a_Value )
{
	TexParameterImpl( a_TextureTarget, a_TextureParameter, a_Value );
}

void Rendering::TexParameterui( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, const uint32_t* a_Value )
{
	TexParameterImpl( a_TextureTarget, a_TextureParameter, a_Value );
}

void Rendering::TextureParameterf( TextureHandle a_Handle, TextureParameter a_TextureParameter, float a_Value )
{
	TextureParameterImpl( a_Handle, a_TextureParameter, a_Value );
}

void Rendering::TextureParameterfv( TextureHandle a_Handle, TextureParameter a_TextureParameter, const float* a_Value )
{
	TextureParameterImpl( a_Handle, a_TextureParameter, a_Value );
}

void Rendering::TextureParameteri( TextureHandle a_Handle, TextureParameter a_TextureParameter, int32_t a_Value )
{
	TextureParameterImpl( a_Handle, a_TextureParameter, a_Value );
}

void Rendering::TextureParameteri( TextureHandle a_Handle, TextureParameter a_TextureParameter, const int32_t* a_Value )
{
	TextureParameterImpl( a_Handle, a_TextureParameter, a_Value );
}

void Rendering::TextureParameterui( TextureHandle a_Handle, TextureParameter a_TextureParameter, uint32_t a_Value )
{
	TextureParameterImpl( a_Handle, a_TextureParameter, a_Value );
}

void Rendering::TextureParameterui( TextureHandle a_Handle, TextureParameter a_TextureParameter, const uint32_t* a_Value )
{
	TextureParameterImpl( a_Handle, a_TextureParameter, a_Value );
}

void Rendering::GenTextures( size_t a_Count, TextureHandle* a_Handles )
{
	while ( a_Count-- > 0 ) a_Handles[ a_Count ] = s_TextureRegistry.Create();
}

void Rendering::BindTexture( TextureTarget a_TextureTarget, TextureHandle a_Handle )
{
	if ( s_TextureRegistry.Bind( a_TextureTarget, a_Handle ) )
	{
		s_ActiveTextureTarget = ( uint32_t )a_TextureTarget;
	}

	auto& ActiveTextureUnit = s_TextureUnits[ s_ActiveTextureUnit ];
	ActiveTextureUnit[ s_ActiveTextureTarget ] = a_Handle;
}

void Rendering::TexImage2D( TextureTarget a_TextureTarget, uint8_t a_MipMapLevel, TextureFormat a_InternalFormat, int32_t a_Width, int32_t a_Height, int32_t a_Border, TextureFormat a_TextureFormat, TextureSetting a_DataLayout, const void* a_Data )
{
	//TextureBuffer& Target = s_TextureRegistry[ s_TextureTargets[ ( uint32_t )a_TextureTarget ] ];
	auto Handle = s_TextureUnits[ s_ActiveTextureUnit ][ ( uint32_t )a_TextureTarget ];
	auto& Target = s_TextureRegistry[ Handle ];
	Target.Data = a_Data;
	Target.Dimensions = { a_Width, a_Height };

	// Need to implement rest of all the settings.
}