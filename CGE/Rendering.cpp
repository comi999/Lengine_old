#include "Rendering.hpp"
#include "ConsoleWindow.hpp"

Rendering::BufferRegistry    Rendering::s_BufferRegistry;
Rendering::ArrayRegistry     Rendering::s_ArrayRegistry;
Rendering::AttributeRegistry Rendering::s_AttributeRegistry;
ArrayHandle                  Rendering::s_BoundArray;
BufferHandle                 Rendering::s_BufferTargets[ 14 ];

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

bool Rendering::UseProgram( ShaderProgram a_ShaderProgram )
{
	return false;
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
	s_BoundArray = a_Handle;
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

