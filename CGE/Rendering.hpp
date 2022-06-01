#pragma once
#include <stdint.h>
#include <array>
#include <vector>
#include <bitset>
#include <type_traits>
#include "Math.hpp"
#include "RenderMode.hpp"
#include "Colour.hpp"
#include "ConsoleWindow.hpp"

// broad phase filtering: remove OBJECTS that will definitely not show up on screen by using encompassing regions and frustum planes
// vertex shader transform vertices into clip space
// everything is now in clip space
// clipping
// move from clip space to ndc space / could be perspective correct/affine/flat
// ndc to screen space
// fragment shader when rasterizing triangle on screen

// MESH:
// Contains:
// - Array of TriangleIndices
// - Array of VertixIndices
// - Array of VertexColours
// - Array of VertexPositions
// - Array of VertexNormals
// - Array of VertexTangents
// - Array of VertexBitangents
// - Array of VertexTexels

// MATERIAL:
// Contains:
// - Shader reference
// - Texture list
// - Can contain solid colour
// - Rendering settings (like ambient colour, etc)

// DRAW CALL:
// A draw call consists of:
// - Mesh reference
// - Material reference
// - Primitive render type: (Point, Line, Triangle)
// - ModelMatrix

// PROCESS:
// - All render components are called and draw calls are created from each one.
// - A draw call is rejected if model exists outside of frustum.
// - Draw calls are inserted based on sorting criteria.
// - Draw queue is processed and render commands are generated.
// - Render commands are sent to rendering thread and processed.
// - Post processing. Allow to render to target texture and use it for post processing.

// SHADER:
// - Vertex shader iterates over input vertex buffer and processes all of them converting them into clip space and copying into output vertex buffer
// - Based on primitive rendering mode (point/line/triangle), clipping based on this is done.
// - As clipping occurs, if in line or triangle mode, lines and triangles are written to an index buffer
// - Extra points generated from clipping are inserted into an extra buffer.

enum class BufferTarget : uint8_t
{
	ARRAY_BUFFER,
	ATOMIC_COUNTER_BUFFER,
	COPY_READ_BUFFER,
	COPY_WRITE_BUFFER,
	DISPATCH_INDIRECT_BUFFER,
	DRAW_INDIRECT_BUFFER,
	ELEMENT_ARRAY_BUFFER,
	PIXEL_PACK_BUFFER,
	PIXEL_UNPACK_BUFFER,
	QUERY_BUFFER,
	SHADER_STORAGE_BUFFER,
	TEXTURE_BUFFER,
	TRANSFORM_FEEDBACK_BUFFER,
	UNIFORM_BUFFER
};

enum class BufferFlag : uint8_t
{
	COLOUR_BUFFER_BIT  = 1u << 0,
	DEPTH_BUFFER_BIT   = 1u << 1,
	ACCUM_BUFFER_BIT   = 1u << 2,
	STENCIL_BUFFER_BIT = 1u << 3
};

inline uint8_t operator |( BufferFlag a_FlagA, BufferFlag a_FlagB )
{
	return static_cast< uint8_t >( a_FlagA ) | static_cast< uint8_t >( a_FlagB );
}

enum class TextureTarget : uint8_t
{
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D
};

enum class TextureWrapMode : uint8_t
{
	REPEAT,
	MIRRORED_REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER
};

enum class TextureParameter : uint8_t
{
	DIMENSION_S,
	DIMENSION_T,
	DIMENSION_R,
	BORDER_COLOUR,
	MIN_FILTER,
	MAG_FILTER,
};

enum class TextureFilterMode : uint8_t
{
	NEAREST,
	LINEAR
};

enum class TextureFormat : uint8_t
{
	RGB,
	RGBA
};

enum class DataType : uint8_t
{
	UNSIGNED_BYTE,
	BYTE,
	UNSIGNED_SHORT,
	SHORT,
	UNSIGNED_INT,
	INT,
	FLOAT,
	DOUBLE
};

enum class DataUsage : uint8_t
{
	STREAM,
	STATIC,
	DYNAMIC,
	STATIC_DRAW,
	READ,
	COPY
};

enum class ShaderType : uint32_t
{
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	INVALID = uint32_t( -1 )
};

enum class ShaderInfo : uint8_t
{
	COMPILE_STATUS,
	INFO_LOG_LENGTH,
	LINK_STATUS
};

typedef uint32_t BufferHandle;
typedef uint32_t ArrayHandle;
typedef uint32_t ShaderHandle;
typedef uint32_t ShaderProgramHandle;

class Rendering;

struct Shader
{
private:

	friend class Rendering;

	static void Empty() { }

public:

	ShaderType Type = ShaderType::INVALID;
	void( *Callback )() = nullptr;
};

class ShaderProgram
{
private:

	friend class Rendering;

	typedef void( *ShaderCallback )();

	struct ShaderEntry
	{
		bool         Set = false;
		ShaderHandle Handle = 0;
		void( *Callback )( ) = nullptr;
	};

	inline ShaderCallback operator[]( ShaderType a_ShaderType ) const
	{
		return Shaders[ ( uint32_t )a_ShaderType ].Callback;
	}

	ShaderEntry Shaders[ 2 ];
};

// Analogous to OpenGL
class Rendering
{
public:

	// Shader IO.
	static Vector4 Position;
	static Vector4 FragColour;

	// Shader functions
	static ShaderHandle CreateShader( ShaderType a_ShaderType );
	static void ShaderSource( ShaderHandle a_ShaderHandle, uint32_t a_Count, const void** a_Sources, uint32_t* a_Lengths );
	static void CompileShader( ShaderHandle a_ShaderHandle );
	static void GetShaderIV( ShaderHandle a_ShaderHandle, ShaderInfo a_ShaderInfo, void* a_Value );
	static void GetShaderInfoLog( ShaderHandle a_ShaderHandle, size_t a_BufferSize, size_t* a_Length, char* a_InfoLog );
	static ShaderProgramHandle CreateProgram();
	static void AttachShader( ShaderProgramHandle a_ShaderProgramHandle, ShaderHandle a_ShaderHandle );
	static void LinkProgram( ShaderProgramHandle a_ShaderProgramHandle );
	static void GetProgramIV( ShaderProgramHandle a_ShaderProgramHandle, ShaderInfo a_ShaderInfo, void* a_Value );
	static void GetProgramInfoLog( ShaderProgramHandle a_ShaderProgramHandle, size_t a_BufferSize, size_t* a_Length, char* a_InfoLog );
	static void DetachShader( ShaderProgramHandle a_ShaderProgramHandle, ShaderHandle a_ShaderHandle );
	static void DeleteShader( ShaderHandle a_ShaderHandle );
	static void DeleteProgram( ShaderProgramHandle a_ShaderProgramHandle );

	static void GenBuffers( uint32_t a_Count, BufferHandle* a_Handles );
	static void BindBuffer( BufferTarget a_BufferBindingTarget, BufferHandle a_Handle );
	static void DeleteBuffers( uint32_t a_Count, BufferHandle* a_Handles );
	static bool IsBuffer( BufferHandle a_Handle );
	//static bool ViewPort( size_t a_X, size_t a_Y, size_t a_Width, size_t a_Height );
	static void UseProgram( ShaderProgramHandle a_ShaderProgramHandle );
	static void Clear( BufferFlag a_Flags );
	static void ClearColour( float a_R, float a_G, float a_B, float a_A );
	static void DrawArrays( RenderMode a_Mode, uint32_t a_Begin, uint32_t a_Count );
	static void BufferData( BufferTarget a_BufferTarget, size_t a_Size, const void* a_Data, DataUsage a_DataUsage );
	static void NamedBufferData( BufferHandle a_Handle, size_t a_Size, const void* a_Data, DataUsage a_DataUsage );
	static void GenVertexArrays( uint32_t a_Count, ArrayHandle* a_Handles );
	static void BindVertexArray( ArrayHandle a_Handle );
	static void DeleteVertexArrays( uint32_t a_Count, ArrayHandle* a_Handles );
	static void EnableVertexAttribArray( uint32_t a_Position );
	static void DisableVertexAttribArray( uint32_t a_Position );
	static void TexParameter( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, uint8_t a_Value );
	static void TexParameter( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, void* a_Value );
	static void GenTextures( size_t a_Count, BufferHandle* a_Handles );
	static void BindTexture( TextureTarget a_TextureTarget, BufferHandle a_Handle );
	//static void TexImage1D( /*something*/ );
	//static void TexImage2D( TextureTarget a_TextureTarget, uint8_t a_MipMapLevel, TextureFormat a_TextureFormat, Texture* a_Texture );
	//static void TexImage3D( /*something*/ );
	static void GenMipmap( TextureTarget a_TextureTarget );
	static void VertexAttribPointer( uint32_t a_Index, uint32_t a_Size, DataType a_DataType, bool a_Normalized, size_t a_Stride, void* a_Offset );
	static void DrawElements( RenderMode a_Mode, size_t a_Count, DataType a_DataType, size_t a_Offset );

	//----------SHADER_ACCESS--------------

	template < uint32_t _Position, typename _Type >
	class In
	{
	public:

		In()
		{
			s_AttributeRegistry[ _Position ]( &s_Value );
		}

		operator _Type& const() const
		{
			return s_Value;
		}

		const _Type* operator->() const
		{
			return &s_Value;
		}

		_Type operator*() const
		{
			return s_Value;
		}

	private:

		static _Type s_Value;
	};

	template < uint32_t _Position, typename _Type >
	class Out
	{
	public:

		Out& operator =( const _Type& a_Value )
		{
			s_Value = a_Value;
		}

		operator _Type& ()
		{
			return s_Value;
		}

		_Type* operator->()
		{
			return &s_Value;
		}

		_Type& operator*() const
		{
			return s_Value;
		}

	private:

		static _Type s_Value;
	};

	class ShaderRegistry
	{
	public:

		ShaderHandle Create()
		{
			size_t Index = 0;
			while ( m_Availability[ Index++ ] );
			m_Availability[ Index - 1 ] = true;
			return Index;
		}

		void Destroy( ShaderHandle a_Handle )
		{
			m_Availability[ a_Handle - 1 ] = false;
			m_Shaders[ a_Handle - 1 ].Callback = nullptr;
			m_Shaders[ a_Handle - 1 ].Type = ShaderType::INVALID;
		}

		inline Shader& operator[]( ShaderHandle a_Handle )
		{
			return m_Shaders[ a_Handle - 1 ];
		}

		inline bool Valid( ShaderHandle a_Handle )
		{
			return a_Handle - 1 < m_Availability.size() && m_Availability[ a_Handle - 1 ];
		}

	private:

		std::bitset< 32 >        m_Availability;
		std::array< Shader, 32 > m_Shaders;
	};

	class ShaderProgramRegistry
	{
	public:

		ShaderProgramHandle Create()
		{
			size_t Index = 0;
			while ( m_Availability[ Index++ ] );
			m_Availability[ Index - 1 ] = true;
			return Index;
		}

		void Destroy( ShaderProgramHandle a_Handle )
		{
			m_Availability[ a_Handle - 1 ] = false;
		
			for ( uint32_t i = 0; i < 2; ++i )
			{
				m_ShaderPrograms[ a_Handle - 1 ].Shaders[ i ].Set = false;
				m_ShaderPrograms[ a_Handle - 1 ].Shaders[ i ].Handle = 0;
			}
		}

		inline ShaderProgram& operator[]( ShaderProgramHandle a_Handle )
		{
			return m_ShaderPrograms[ a_Handle - 1 ];
		}

		inline bool Valid( ShaderProgramHandle a_Handle )
		{
			return a_Handle - 1 < m_Availability.size() && m_Availability[ a_Handle - 1 ];
		}

	private:

		std::bitset< 32 >               m_Availability;
		std::array< ShaderProgram, 32 > m_ShaderPrograms;
	};

private:

	//----------IMPLEMENTATION-------------

	struct VertexAttribute
	{
		bool         Enabled;
		BufferHandle Buffer;
		uint32_t     Offset;
		uint32_t     Stride;
		uint8_t      Normalized : 1;
		uint8_t      Size       : 4;
		uint8_t      Type       : 3;
	};
	class AttributeIterator
	{
	public:

		AttributeIterator()
			: m_Output( nullptr )
			, m_Stride( 0 )
			, m_Begin( nullptr )
			, m_Data( nullptr )
		{ }

		AttributeIterator( VertexAttribute a_VertexAttribute )
		{
			m_Output = OutputArray(
				std::in_place_type< std::make_integer_sequence< size_t, 256 > > )[
					( a_VertexAttribute.Type << 5 ) |
						( a_VertexAttribute.Size << 1 ) |
						( a_VertexAttribute.Normalized ) ];

			m_Begin = 
				s_BufferRegistry[ a_VertexAttribute.Buffer ].data() + 
				a_VertexAttribute.Offset;

			m_Data = m_Begin;

			m_Stride = a_VertexAttribute.Stride;
		}

		inline void Reset()
		{
			m_Data = m_Begin;
		}

		inline void Clear()
		{
			m_Output = nullptr;
			m_Stride = 0;
			m_Data = nullptr;
		}

		inline AttributeIterator& operator =( VertexAttribute a_VertexAttribute )
		{
			m_Output = OutputArray(
				std::in_place_type< std::make_integer_sequence< size_t, 256 > > )[
					( a_VertexAttribute.Type << 5 ) |
						( a_VertexAttribute.Size << 1 ) |
						( a_VertexAttribute.Normalized ) ];

			m_Begin =
				s_BufferRegistry[ a_VertexAttribute.Buffer ].data() +
				a_VertexAttribute.Offset;

			m_Data = m_Begin;

			m_Stride = a_VertexAttribute.Stride;

			return *this;
		}

		inline operator bool() const
		{
			return m_Data;
		}

		inline auto& operator++()
		{
			m_Data += m_Stride;
			return *this;
		}

		inline void operator()( void* o_Output ) const
		{
			m_Output( m_Data, o_Output );
		}

	private:

		template < DataType _DataType > struct DataTypeImpl { using Type = void; };
		template <> struct DataTypeImpl< DataType::UNSIGNED_BYTE  > { using Type = uint8_t;  };
		template <> struct DataTypeImpl< DataType::BYTE           > { using Type = int8_t;   };
		template <> struct DataTypeImpl< DataType::UNSIGNED_SHORT > { using Type = uint16_t; };
		template <> struct DataTypeImpl< DataType::SHORT          > { using Type = int16_t;  };
		template <> struct DataTypeImpl< DataType::UNSIGNED_INT   > { using Type = uint32_t; };
		template <> struct DataTypeImpl< DataType::INT            > { using Type = int32_t;  };
		template <> struct DataTypeImpl< DataType::FLOAT          > { using Type = float;    };
		template <> struct DataTypeImpl< DataType::DOUBLE         > { using Type = double;   };

		template < DataType _DataType >
		using GetDataType = typename DataTypeImpl< _DataType >::Type;

		template < typename _Type >
		static constexpr float Normalizer( _Type a_Value )
		{
			if ( std::is_integral_v< _Type > )
			{
				if constexpr ( std::is_unsigned_v< _Type > )
				{
					constexpr float InvMax = 1.0f / static_cast< _Type >( -1 );
					return InvMax * a_Value;
				}
				else
				{
					constexpr double InvMin = 1.0f / ( static_cast< size_t >( 1 ) << ( sizeof( _Type ) * 8 - 1 ) );
					constexpr double InvMax = 1.0f / ( ( static_cast< size_t >( 1 ) << ( sizeof( _Type ) * 8 - 1 ) ) - 1 );
					return ( a_Value < 0 ? InvMin : InvMax ) * a_Value;
				}
			}
			else
			{
				return a_Value;
			}
		}

		template < typename _Type, size_t _Size, bool _Normalize, size_t... Idxs >
		static void Cast( const uint8_t* a_Data, void* o_Output, std::in_place_type_t< std::index_sequence< Idxs... > > )
		{
			if constexpr ( _Normalize )
			{
				( ( ( *reinterpret_cast< Vector< float, _Size >* >( o_Output ) )[ Idxs ] = Normalizer( reinterpret_cast< const _Type* >( a_Data )[ Idxs ] ) ), ... );
			}
			else
			{
				( ( ( *reinterpret_cast< Vector< float, _Size >* >( o_Output ) )[ Idxs ] = reinterpret_cast< const _Type* >( a_Data )[ Idxs ] ), ... );
			}
		}

		template < uint8_t _Interface >
		static void Output( const uint8_t* a_Data, void* o_Output )
		{
			constexpr uint8_t Type = ( _Interface & 0b11100000 ) >> 5;
			constexpr uint8_t Size = ( _Interface & 0b00011110 ) >> 1;
			constexpr uint8_t Norm = ( _Interface & 0b00000001 ) >> 0;

			Cast< GetDataType< static_cast< DataType >( Type ) >, Size, Norm >( a_Data, o_Output, std::in_place_type< std::make_index_sequence< Size > > );
		}

		typedef void( *OutputFunc )( const uint8_t*, void* );

		template < size_t... Idxs >
		static OutputFunc* OutputArray( std::in_place_type_t< std::integer_sequence< size_t, Idxs... > > )
		{
			static OutputFunc Funcs[ 256 ] = { Output< static_cast< uint8_t >( Idxs ) >... };
			return &Funcs[ 0 ];
		}

		OutputFunc m_Output;
		uint32_t   m_Stride;
		uint8_t*   m_Begin;
		uint8_t*   m_Data;
	};

	typedef std::vector< uint8_t > Buffer;
	typedef std::array< VertexAttribute, 8 > Array;

	class BufferRegistry
	{
	public:

		BufferHandle Create()
		{
			size_t Index = 0;
			while ( m_Availability[ Index++ ] );
			m_Availability[ Index - 1 ] = true;
			return Index;
		}

		void Destroy( BufferHandle a_Handle )
		{
			m_Availability[ a_Handle - 1 ] = false;
			m_Buffers[ a_Handle - 1 ].clear();
		}

		inline Buffer& operator[]( BufferHandle a_Handle )
		{
			return m_Buffers[ a_Handle - 1 ];
		}

		inline bool Valid( BufferHandle a_Handle )
		{
			return a_Handle - 1 < m_Availability.size() && m_Availability[ a_Handle - 1 ];
		}

	private:

		std::bitset< 32 >        m_Availability;
		std::array< Buffer, 32 > m_Buffers;
	};
	class ArrayRegistry
	{
	public:

		ArrayHandle Create()
		{
			uint32_t Index = 0;
			while ( m_Availability[ Index++ ] );
			m_Availability[ Index - 1 ] = true;
			return Index;
		}

		void Destroy( ArrayHandle a_Handle )
		{
			m_Availability[ a_Handle - 1 ] = false;
		}

		inline Array& operator[]( ArrayHandle a_Handle )
		{
			return m_Arrays[ a_Handle - 1 ];
		}

		inline bool Valid( ArrayHandle a_Handle )
		{
			return a_Handle - 1 < m_Availability.size() && m_Availability[ a_Handle - 1 ];
		}

	private:

		std::bitset< 32 >       m_Availability;
		std::array< Array, 32 > m_Arrays;
	};
	class AttributeRegistry
	{
	public:

		inline void Increment( uint32_t a_Position )
		{
			++m_VertexAttributes[ a_Position ];
		}

		inline void IncrementAll()
		{
			// In future, use a static incrementing function.

			++m_VertexAttributes[ 0 ];
			++m_VertexAttributes[ 1 ];
			++m_VertexAttributes[ 2 ];
			++m_VertexAttributes[ 3 ];
			++m_VertexAttributes[ 4 ];
			++m_VertexAttributes[ 5 ];
			++m_VertexAttributes[ 6 ];
			++m_VertexAttributes[ 7 ];
		}

		inline void Reset( uint32_t a_Position )
		{
			m_VertexAttributes[ a_Position ].Reset();
		}

		inline void ResetAll()
		{
			m_VertexAttributes[ 0 ].Reset();
			m_VertexAttributes[ 1 ].Reset();
			m_VertexAttributes[ 2 ].Reset();
			m_VertexAttributes[ 3 ].Reset();
			m_VertexAttributes[ 4 ].Reset();
			m_VertexAttributes[ 5 ].Reset();
			m_VertexAttributes[ 6 ].Reset();
			m_VertexAttributes[ 7 ].Reset();
		}

		inline void Value( uint32_t a_Position, void* o_Output )
		{
			m_VertexAttributes[ a_Position ]( o_Output );
		}

		inline AttributeIterator& operator[]( uint32_t a_Position )
		{
			return m_VertexAttributes[ a_Position ];
		}

	private:

		AttributeIterator m_VertexAttributes[ 8 ];
	};

	static void DrawTriangleImpl( Vector4 a_P0, Vector4 a_P1, Vector4 a_P2, Vector4 a_C0, Vector4 a_C1, Vector4 a_C2 )
	{
		// Correct Y
		a_P0.y = -a_P0.y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();
		a_P1.y = -a_P1.y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();
		a_P2.y = -a_P2.y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();

		// Sort corners.
		if ( a_P0.y < a_P1.y )
		{
			std::swap( a_P0, a_P1 );
			std::swap( a_C0, a_C1 );
		}

		if ( a_P0.y < a_P2.y )
		{
			std::swap( a_P0, a_P2 );
			std::swap( a_C0, a_C2 );
		}

		if ( a_P1.y < a_P2.y )
		{
			std::swap( a_P1, a_P2 );
			std::swap( a_C1, a_C2 );
		}

		if ( a_P0.y == a_P2.y )
		{
			return;
		}

		// Align y.
		a_P0.y = static_cast< int >( a_P0.y );
		a_P1.y = static_cast< int >( a_P1.y );
		a_P2.y = static_cast< int >( a_P2.y );

		// Find M
		float L = ( a_P1.y - a_P2.y ) / ( a_P0.y - a_P2.y );
		Vector4 PM = Math::Lerp( L, a_P2, a_P0 );
		PM.y = static_cast< int >( PM.y );
		Vector4 CM = Math::Lerp( L, a_C2, a_C0 );

		// Swap P1 and M if M is not on right.
		if ( PM.x < a_P1.x )
		{
			std::swap( PM, a_P1 );
			std::swap( CM, a_C1 );
		}

		float SpanX, SpanY, y;
		Vector4 PStepL, PStepR, PBegin, PStep, PL, PR;
		Vector4 CStepL, CStepR, CBegin, CStep, CL, CR;

		if ( a_P2.y != a_P1.y )
		{
			SpanY = 1.0f / ( a_P1.y - a_P2.y );

			PStepL = a_P1 - a_P2; PStepL *= SpanY;
			PStepR = PM - a_P2; PStepR *= SpanY;
			PL = a_P2;
			PR = a_P2;

			CStepL = a_C1 - a_C2; CStepL *= SpanY;
			CStepR = CM - a_C2; CStepR *= SpanY;
			CL = a_C2;
			CR = a_C2;

			y = a_P2.y;

			for ( ; y < a_P1.y; ++y )
			{
				PL += PStepL;
				PR += PStepR;
				CL += CStepL;
				CR += CStepR;

				SpanX = 1.0f / ( PR.x - PL.x );
				PBegin = PL;
				PStep = ( PR - PL ) * SpanX;
				CBegin = CL;
				CStep = ( CR - CL ) * SpanX;

				for ( ; PBegin.x < PR.x; PBegin += PStep, CBegin += CStep )
				{
					Vector4 col = CBegin / PBegin.w;
					col *= 255;
					ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetColour( { PBegin.x, y }, Colour( col.x, col.y, col.z, col.w ) );
				}
			}
		}
		else
		{
			y = a_P1.y;
			PL = a_P1, PR = PM;
			CL = a_C1, CR = CM;
		}

		if ( a_P1.y != a_P0.y )
		{
			SpanY = 1.0f / ( a_P0.y - a_P1.y );

			PStepL = a_P0 - a_P1; PStepL *= SpanY;
			PStepR = a_P0 - PM; PStepR *= SpanY;

			CStepL = a_C0 - a_C1; CStepL *= SpanY;
			CStepR = a_C0 - CM; CStepR *= SpanY;

			for ( ; y < a_P0.y; ++y )
			{
				PL += PStepL;
				PR += PStepR;
				CL += CStepL;
				CR += CStepR;

				SpanX = 1.0f / ( PR.x - PL.x );
				PBegin = PL;
				PStep = ( PR - PL ) * SpanX;
				CBegin = CL;
				CStep = ( CR - CL ) * SpanX;

				for ( ; PBegin.x < PR.x; PBegin += PStep, CBegin += CStep )
				{
					Vector4 col = CBegin / PBegin.w;
					col *= 255;
					ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetColour( { PBegin.x, y }, Colour( col.x, col.y, col.z, col.w ) );
				}
			}
		}
	}

	static void DrawArraysImpl_Triangle( uint32_t a_Begin, uint32_t a_Count )
	{
		auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
		uint32_t Begin = a_Begin;
		uint32_t End = a_Begin + a_Count;

		std::vector< Vector4 > Positions;
		Positions.resize( a_Count );
		auto PositionsBegin = Positions.begin();

		std::vector< Vector4 > Colours;
		Colours.resize( a_Count );
		auto ColoursBegin = Colours.begin();

		// Process vertices.
		do
		{
			for ( uint32_t i = 0; i < 3 && Begin < End; ++i, ++Begin, ++PositionsBegin, ++ColoursBegin, s_AttributeRegistry.IncrementAll() )
			{
				ActiveProgram[ ShaderType::VERTEX_SHADER ]();
				*PositionsBegin = Rendering::Position;
				Rendering::Out< 0, Vector4 > col;
				*ColoursBegin = *col;
			}

		} while ( Begin < End );

		Begin = a_Begin;
		PositionsBegin = Positions.begin();
		ColoursBegin = Colours.begin();

		// Process fragments.
		Vector4 P[ 3 ];
		Vector4 C[ 3 ];

		Vector4 Scale( 0.0f, 0.0f, 0.0f, 1.0f );
		Scale.x = 0.5f * ConsoleWindow::GetCurrentContext()->GetWidth();
		Scale.y = 0.5f * ConsoleWindow::GetCurrentContext()->GetHeight();

		Vector4 Trans = Scale;
		Trans.w = 0.0f;

		do
		{
			for ( uint32_t i = 0; i < 3 && Begin < End; ++i, ++Begin, ++PositionsBegin, ++ColoursBegin )
			{
				P[ i ] = *PositionsBegin;
				C[ i ] = *ColoursBegin;
				
				// Convert to screen space.
				P[ i ] *= Scale;
				P[ i ] += Trans;

				//ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetColour( { Points[ i ].x, Points[ i ].y }, Colour::GREEN );
				
			}

			// Interpolate.
			DrawTriangleImpl( P[ 0 ], P[ 1 ], P[ 2 ], C[ 0 ], C[ 1 ], C[ 2 ] );

		}
		while( Begin < End );
	}

	static BufferRegistry        s_BufferRegistry;
	static ArrayRegistry         s_ArrayRegistry;
	static AttributeRegistry     s_AttributeRegistry;
	static ShaderRegistry        s_ShaderRegistry;
	static ShaderProgramRegistry s_ShaderProgramRegistry;
	static ArrayHandle           s_BoundArray;
	static ShaderProgramHandle   s_ActiveShaderProgram;
	static BufferHandle          s_BufferTargets[ 14 ];
};

template < uint32_t _Position, typename _Type >
_Type Rendering::In< _Position, _Type >::s_Value;

template < uint32_t _Position, typename _Type >
_Type Rendering::Out< _Position, _Type >::s_Value;