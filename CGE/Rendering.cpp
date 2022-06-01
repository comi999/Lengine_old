#include "Rendering.hpp"
#include "ConsoleWindow.hpp"

Vector4                          Rendering::Position;
Vector4                          Rendering::FragColour;
Rendering::BufferRegistry        Rendering::s_BufferRegistry;
Rendering::ArrayRegistry         Rendering::s_ArrayRegistry;
Rendering::AttributeRegistry     Rendering::s_AttributeRegistry;
Rendering::ShaderRegistry        Rendering::s_ShaderRegistry;
Rendering::ShaderProgramRegistry Rendering::s_ShaderProgramRegistry;
ArrayHandle                      Rendering::s_BoundArray;
ShaderProgramHandle              Rendering::s_ActiveShaderProgram;
BufferHandle                     Rendering::s_BufferTargets[ 14 ];

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
// needs work
void Rendering::Clear( BufferFlag a_Flags )
{
	if ( static_cast< uint8_t >( a_Flags ) & 
		 static_cast< uint8_t >( BufferFlag::COLOUR_BUFFER_BIT ) )
	{

	}

	if ( static_cast< uint8_t >( a_Flags ) &
		 static_cast< uint8_t >( BufferFlag::DEPTH_BUFFER_BIT ) )
	{

	}

	if ( static_cast< uint8_t >( a_Flags ) &
		 static_cast< uint8_t >( BufferFlag::ACCUM_BUFFER_BIT ) )
	{

	}

	if ( static_cast< uint8_t >( a_Flags ) &
		 static_cast< uint8_t >( BufferFlag::STENCIL_BUFFER_BIT ) )
	{

	}
}

void Rendering::ClearColour( float a_R, float a_G, float a_B, float a_A )
{
	ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetBuffer( { 
		static_cast< unsigned char >( 255u * a_R ), 
		static_cast< unsigned char >( 255u * a_G ), 
		static_cast< unsigned char >( 255u * a_B ), 
		static_cast< unsigned char >( 255u * a_A ) } );
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
			DrawArraysImpl_Triangle( a_Begin, a_Count );
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
		s_BoundArray = 0;
		return;
	}

	s_BoundArray = a_Handle;
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
		if ( s_BoundArray == a_Handles[ a_Count ] )
		{
			s_BoundArray = 0;
		}

		// Destroy buffer.
		s_ArrayRegistry.Destroy( a_Handles[ a_Count ] );
	}
}

void Rendering::EnableVertexAttribArray( uint32_t a_Position )
{
	s_ArrayRegistry[ s_BoundArray ][ a_Position ].Enabled = true;
}

void Rendering::DisableVertexAttribArray( uint32_t a_Position )
{
	s_ArrayRegistry[ s_BoundArray ][ a_Position ].Enabled = false;
}

void Rendering::VertexAttribPointer( uint32_t a_Index, uint32_t a_Size, DataType a_DataType, bool a_Normalized, size_t a_Stride, void* a_Offset )
{
	auto& Attributes = s_ArrayRegistry[ s_BoundArray ][ a_Index ];
	Attributes.Buffer = s_BufferTargets[ ( uint32_t )BufferTarget::ARRAY_BUFFER ];
	Attributes.Size = a_Size;
	Attributes.Normalized = a_Normalized;
	Attributes.Type = ( uint32_t )a_DataType;
	Attributes.Stride = a_Stride;
	Attributes.Enabled = false;
	Attributes.Offset = ( uint32_t )a_Offset;
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
	Program.Shaders[ ( uint32_t )Shader.Type ].Handle = a_ShaderHandle;
	Program.Shaders[ ( uint32_t )Shader.Type ].Set = true;
}

void Rendering::LinkProgram( ShaderProgramHandle a_ShaderProgramHandle )
{
	auto& Program = s_ShaderProgramRegistry[ a_ShaderProgramHandle ];
	
	for ( uint32_t i = 0; i < 2; ++i )
	{
		auto& Entry = Program.Shaders[ i ];

		if ( !Entry.Set )
		{
			Entry.Callback = Shader::Empty;
			continue;
		}

		Program.Shaders[ i ].Callback = s_ShaderRegistry[ Entry.Handle ].Callback;
	}
}

void Rendering::GetProgramIV( ShaderProgramHandle a_ShaderProgramHandle, ShaderInfo a_ShaderInfo, void* a_Value )
{}

void Rendering::GetProgramInfoLog( ShaderProgramHandle a_ShaderProgramHandle, size_t a_BufferSize, size_t * a_Length, char* a_InfoLog )
{}

void Rendering::DetachShader( ShaderProgramHandle a_ShaderProgramHandle, ShaderHandle a_ShaderHandle )
{
	auto& Program = s_ShaderProgramRegistry[ a_ShaderProgramHandle ];
	auto& Shader  = s_ShaderRegistry[ a_ShaderHandle ];
	auto& Entry   = Program.Shaders[ ( uint32_t )Shader.Type ];
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