#pragma once
//#include "RenderPipeline.hpp"
#include <stdint.h>
#include <array>
#include <vector>
#include <bitset>
#include <type_traits>
#include "Math.hpp"
#include "RenderMode.hpp"
#include "Colour.hpp"

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

enum class DataType : uint32_t
{
	UNSIGNED_BYTE,
	BYTE,
	UNSIGNED_INT,
	INT,
	FLOAT
};

enum class DataUsage : uint8_t
{
	STREAM,
	STATIC,
	DYNAMIC,
	DRAW,
	READ,
	COPY
};

typedef uint32_t BufferHandle;
typedef uint32_t ArrayHandle;
typedef uint32_t ShaderProgram;

// Analogous to OpenGL
class Rendering
{
public:

	static void GenBuffers( uint32_t a_Count, BufferHandle* a_Handles );
	static void BindBuffer( BufferTarget a_BufferBindingTarget, BufferHandle a_Handle );
	static void DeleteBuffers( uint32_t a_Count, BufferHandle* a_Handles );
	static bool IsBuffer( BufferHandle a_Handle );
	//static bool ViewPort( size_t a_X, size_t a_Y, size_t a_Width, size_t a_Height );
	static bool UseProgram( ShaderProgram a_ShaderProgram );
	static void Clear( BufferFlag a_Flags );
	static void ClearColour( float a_R, float a_G, float a_B, float a_A );
	static void DrawArrays( RenderMode a_Mode, size_t a_Start, size_t a_Count );
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


private:

	struct VertexAttribute
	{
		bool         Enabled;
		BufferHandle Buffer;
		uint32_t     Offset;
		uint32_t     Stride;

		union
		{
			struct
			{
				uint32_t Type : 3;
				uint32_t Size : 4;
				uint32_t Normalized : 1;
			};

			uint8_t Interface;
		};
	};

	static constexpr size_t s = sizeof( double );

	typedef std::vector< uint8_t > Buffer;
	typedef std::array< VertexAttribute, 8 > Array;

	class BufferRegistry
	{
	public:

		inline BufferHandle Create()
		{
			size_t Index = 0;
			while ( m_Availability[ Index++ ] );
			m_Availability[ Index - 1 ] = true;
			return Index;
		}

		inline void Destroy( BufferHandle a_Handle )
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

		inline ArrayHandle Create()
		{
			uint32_t Index = 0;
			while ( m_Availability[ Index++ ] );
			m_Availability[ Index - 1 ] = true;
			return Index;
		}

		inline void Destroy( ArrayHandle a_Handle )
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
	public:
	//----------IMPLEMENTATION-------------
	class AttributeIterator
	{
	public:

		AttributeIterator( VertexAttribute a_VertexAttribute )
		{
			m_Output = GetOutputFunc( a_VertexAttribute.Interface );
			m_Data = 
				s_BufferRegistry[ a_VertexAttribute.Buffer ].data() + 
				a_VertexAttribute.Offset;
			m_Stride = a_VertexAttribute.Stride;
		}

		inline auto& operator++()
		{
			m_Data += m_Stride;
			return *this;
		}

		inline void operator()( void* o_Output )
		{
			m_Output( m_Data, o_Output );
		}

	private:

		typedef void( *OutputFunc )( uint8_t*, void* );

		template < DataType _DataType > struct DataTypeImpl { using Type = void; };
		template <> struct DataTypeImpl< DataType::BYTE          > { using Type = int8_t;   };
		template <> struct DataTypeImpl< DataType::UNSIGNED_BYTE > { using Type = uint8_t;  };
		template <> struct DataTypeImpl< DataType::INT           > { using Type = int16_t;  };
		template <> struct DataTypeImpl< DataType::UNSIGNED_INT  > { using Type = uint16_t; };
		template <> struct DataTypeImpl< DataType::FLOAT         > { using Type = float;    };
		
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
					constexpr float InvMin = 1.0f / ( 1 << ( sizeof( _Type ) * 8 - 1 ) );
					constexpr float InvMax = 1.0f / ( ( 1 << ( sizeof( _Type ) * 8 - 1 ) ) - 1 );
					return ( a_Value < 0 ? InvMin : InvMax ) * a_Value;
				}
			}
			else
			{
				return a_Value;
			}
		}

		template < typename _Type, size_t _Size, bool _Normalize, size_t... Idxs >
		static void Cast( uint8_t* a_Data, void* o_Output, std::in_place_type_t< std::index_sequence< Idxs... > > )
		{
			using Vector_t = Vector< float, _Size / sizeof( _Type ) >;
			auto* Data = reinterpret_cast< _Type* >( a_Data );
			auto& Vec  = *reinterpret_cast< Vector_t* >( o_Output );

			Vec = { ( _Normalize ? Normalizer( Data[ Idxs ] ) : Data[ Idxs ] )... };
		}

		template < uint8_t _Interface >
		static void Output( uint8_t* a_Data, void* o_Output )
		{/*
			static constexpr uint8_t Type = ( _Interface & 0b11100000 ) >> 5;
			static constexpr uint8_t Size = ( ( _Interface & 0b11110 ) >> 1 ) / 4;
			static constexpr uint8_t Norm = _Interface & 0b1;*/

			struct Interface
			{
				union
				{
					struct
					{
						uint32_t Type : 3;
						uint32_t Size : 4;
						uint32_t Normalized : 1;
					};

					uint8_t someting;
				};

			};

			static constexpr Interface _interface = _Interface;
			static constexpr uint8_t Type = _interface.Type;
			static constexpr uint8_t Size = _interface.Size;
			static constexpr uint8_t Norm = _interface.Normalize;

			//using Type_t = GetDataType< static_cast< DataType >( _interface.Type ) >;
			//using Vector_t = Vector< float, _interface.Size >;

			
			

			//Cast< Type_t, _interface.Size, _interface.Normalize >( a_Data, o_Output, std::in_place_type< std::make_index_sequence< _interface.Size > > );
		}

		

		template < size_t... Idxs >
		static OutputFunc* GetOutputFuncArray( std::in_place_type_t< std::integer_sequence< size_t, Idxs... > > )
		{
			static OutputFunc Funcs[ 256 ] = { Output< static_cast< uint8_t >( Idxs ) >... };
			return &Funcs[ 0 ];
		}

		static OutputFunc GetOutputFunc( uint8_t a_Interface )
		{
			static OutputFunc* Funcs = GetOutputFuncArray( std::in_place_type< std::make_integer_sequence< size_t, 256 > > );
			return Funcs[ a_Interface ];
		}

		OutputFunc m_Output;
		uint32_t   m_Stride;
		uint8_t*   m_Data;
	};

	static auto CreateSomething()
	{
		VertexAttribute v;
		v.Buffer = 1;
		v.Enabled = true;
		v.Normalized = true;
		v.Offset = 0;
		v.Size = 3;
		v.Stride = 3;
		v.Type = (uint32_t)DataType::UNSIGNED_BYTE;
		return v;
	}

	template < typename _Type, size_t _Attribs, size_t _Size, size_t _Stride >
	static void DrawArraysImpl( uint32_t a_Start, uint32_t a_Count )
	{
		//auto& ActiveArray = s_ArrayRegistry[ s_BoundArray ];

		//// Per Primitive Triangle in this case.
		//Vector< _Type, _Size > P[  ];

		//for ( size_t End = a_Start + a_Count; a_Start < End; )
		//{
		//	for ( uint32_t i = 0; i < 3; ++i )
		//	{

		//	}

		//	for ( uint32_t i = 0; i < ActiveArray.size(); ++i )
		//	{
		//		auto& Attrib = ActiveArray[ i ];
		//		
		//		if ( !Attrib.Enabled )
		//		{
		//			continue;
		//		}

		//		auto& Buffer = s_BufferRegistry[ Attrib.Buffer ];


		//	}
		//}
	}

	static BufferRegistry s_BufferRegistry;
	static ArrayRegistry  s_ArrayRegistry;
	static ArrayHandle    s_BoundArray;
	static BufferHandle   s_BufferTargets[ 14 ];
};