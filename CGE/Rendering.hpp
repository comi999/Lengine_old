#pragma once
#include <stdint.h>
#include <array>
#include <vector>
#include <bitset>
#include <type_traits>
#include <map>
#include "Math.hpp"
#include "RenderMode.hpp"
#include "Colour.hpp"
#include "ConsoleWindow.hpp"
#include "Hash.hpp"
#include "Utilities.hpp"
#include "Rect.hpp"

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
	TEXTURE_3D,
	TEXTURE_1D_ARRAY,
	TEXTURE_2D_ARRAY,
	TEXTURE_2D_MULTISAMPLE,
	TEXTURE_2D_MULTISAMPLE_ARRAY,
	TEXTURE_CUBE_MAP, 
	TEXTURE_CUBE_MAP_ARRAY,
	TEXTURE_RECTANGLE,
};

enum class TextureParameter : uint8_t
{
	DEPTH_STENCIL_TEXTURE_MODE, 
	TEXTURE_BASE_LEVEL, 
	TEXTURE_BORDER_COLOUR,
	TEXTURE_COMPARE_FUNC, 
	TEXTURE_COMPARE_MODE, 
	TEXTURE_LOD_BIAS, 
	TEXTURE_MIN_FILTER, 
	TEXTURE_MAG_FILTER, 
	TEXTURE_MIN_LOD, 
	TEXTURE_MAX_LOD, 
	TEXTURE_MAX_LEVEL, 
	TEXTURE_SWIZZLE_R, 
	TEXTURE_SWIZZLE_G, 
	TEXTURE_SWIZZLE_B, 
	TEXTURE_SWIZZLE_A, 
	TEXTURE_SWIZZLE_RGBA,
	TEXTURE_WRAP_S, 
	TEXTURE_WRAP_T, 
	TEXTURE_WRAP_R
};

enum class TextureSetting : uint32_t
{
	// Depth Stencil Texture Mode
	DEPTH_COMPONENT,
	STENCIL_INDEX,

	// Wrap modes
	CLAMP_TO_EDGE, 
	CLAMP_TO_BORDER, 
	MIRRORED_REPEAT, 
	REPEAT,
	MIRROR_CLAMP_TO_EDGE,

	// Filter modes
	NEAREST,
	LINEAR,

	// Texture compare modes
	COMPARE_REF_TO_TEXTURE,
	NONE,

	// Texture LOD bias
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR,

	// Comparison
	LEQUAL,
	GEQUAL,
	LESS,
	GREATER,
	EQUAL,
	NOT_EQUAL,
	ALWAYS,
	NEVER,

	// Texture swizzle
	RED,
	GREEN,
	BLUE,
	ALPHA,
	ZERO,
	ONE,

	// Texture data format
	UNSIGNED_BYTE, 
	UNSIGNED_SHORT_5_6_5, 
	UNSIGNED_SHORT_4_4_4_4, 
	UNSIGNED_SHORT_5_5_5_1,
};

enum class TextureFormat : uint8_t
{
	ALPHA,
	LUMINANCE,
	LUMINANCE_ALPHA,
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

enum class RenderSetting
{
	DEPTH_TEST,
	CULL_FACE,
	// Incomplete
};

enum class CullFaceMode
{
	FRONT,
	BACK,
	FRONT_AND_BACK,
};

#define DefineShader( Name ) \
void Shader_##Name ();       \
template <> void* ShaderAddress< "Shader_"#Name##_H > = Shader_##Name; \
void Shader_##Name ()

#define Uniform( Type, Name ) auto& ##Name = Rendering::Uniform< crc32_cpt( __FUNCTION__ ), Type, #Name##_H >::Value()
#define Attribute( Location, Type, Name ) auto& ##Name = Rendering::Attribute< Location, Type >::Value()
#define Varying_In( Type, Name ) auto& ##Name = Rendering::Varying< crc32_cpt( __FUNCTION__ ), Type, #Name##""_H >::In()
#define Varying_Out( Type, Name ) auto& ##Name = Rendering::Varying< crc32_cpt( __FUNCTION__ ), Type, #Name##""_H >::Out()
#define InOut( Type, Name ) auto& ##Name = Rendering::InOut< Type, #Name##""_H >::Value()

template < Hash _ShaderName >
void* ShaderAddress = nullptr;

typedef uint32_t BufferHandle;
typedef uint32_t ArrayHandle;
typedef uint32_t TextureHandle;
typedef uint32_t ShaderHandle;
typedef uint32_t ShaderProgramHandle;

struct Sampler2D
{
	typedef Vector4 Output;
	typedef Vector2 Input;
	static constexpr TextureTarget Target = TextureTarget::TEXTURE_2D;
	uint32_t Location;
};

class Rendering;

struct Shader
{
public:

	template < Hash _Shader >
	static void*& GetCallback()
	{
		static void* Ptr = nullptr;
		return Ptr;
	}

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
		return m_Shaders[ ( uint32_t )a_ShaderType ].Callback;
	}

	ShaderEntry m_Shaders[ 2 ];

	std::map< Hash, uint32_t >  m_UniformLocations;
	std::vector< void* >        m_Uniforms;
};

class Rendering
{
public:

	// Vertex out variables.
	inline static Vector4 Position;

	// Fragment in variables.
private:

	inline static Vector4 _FragCoord;

public:

	inline static const Vector4& FragCoord = _FragCoord;

	// Fragment out variables.
	inline static Vector4 FragColour;
	inline static float& FragDepth = FragColour.z;

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

	static void Init();
	static void GenBuffers( uint32_t a_Count, BufferHandle* a_Handles );
	static void BindBuffer( BufferTarget a_BufferBindingTarget, BufferHandle a_Handle );
	static void DeleteBuffers( uint32_t a_Count, BufferHandle* a_Handles );
	static bool IsBuffer( BufferHandle a_Handle );
	//static bool ViewPort( size_t a_X, size_t a_Y, size_t a_Width, size_t a_Height );
	static void UseProgram( ShaderProgramHandle a_ShaderProgramHandle );
	static void Clear( uint8_t a_Flags );
	static void ClearColour( float a_R, float a_G, float a_B, float a_A );
	static void ClearDepth( float a_ClearDepth );
	static void DrawArrays( RenderMode a_Mode, uint32_t a_Begin, uint32_t a_Count );
	static void BufferData( BufferTarget a_BufferTarget, size_t a_Size, const void* a_Data, DataUsage a_DataUsage );
	static void NamedBufferData( BufferHandle a_Handle, size_t a_Size, const void* a_Data, DataUsage a_DataUsage );
	static void GenVertexArrays( uint32_t a_Count, ArrayHandle* a_Handles );
	static void BindVertexArray( ArrayHandle a_Handle );
	static void DeleteVertexArrays( uint32_t a_Count, ArrayHandle* a_Handles );
	static void EnableVertexAttribArray( uint32_t a_Position );
	static void DisableVertexAttribArray( uint32_t a_Position );
	static void VertexAttribPointer( uint32_t a_Index, uint32_t a_Size, DataType a_DataType, bool a_Normalized, size_t a_Stride, void* a_Offset );
	static void DrawElements( RenderMode a_Mode, size_t a_Count, DataType a_DataType, size_t a_Offset );
	static void Enable( RenderSetting a_RenderSetting );
	static void Disable( RenderSetting a_RenderSetting );
	static void CullFace( CullFaceMode a_CullFace );

	// Textures
	static void ActiveTexture( uint32_t a_ActiveTexture );	
	static void GenTextures( size_t a_Count, TextureHandle* a_Handles );
	static void BindTexture( TextureTarget a_TextureTarget, TextureHandle a_Handle );
	static void TexParameterf( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, float a_Value );
	static void TexParameterfv( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, const float* a_Value );
	static void TexParameteri( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, int32_t a_Value );
	static void TexParameteri( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, const int32_t* a_Value );
	static void TexParameterui( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, uint32_t a_Value );
	static void TexParameterui( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, const uint32_t* a_Value );
	static void TextureParameterf( TextureHandle a_Handle, TextureParameter a_TextureParameter, float a_Value );
	static void TextureParameterfv( TextureHandle a_Handle, TextureParameter a_TextureParameter, const float* a_Value );
	static void TextureParameteri( TextureHandle a_Handle, TextureParameter a_TextureParameter, int32_t a_Value );
	static void TextureParameteri( TextureHandle a_Handle, TextureParameter a_TextureParameter, const int32_t* a_Value );
	static void TextureParameterui( TextureHandle a_Handle, TextureParameter a_TextureParameter, uint32_t a_Value );
	static void TextureParameterui( TextureHandle a_Handle, TextureParameter a_TextureParameter, const uint32_t* a_Value );
	// Need to look into the Tex/ture/ParameterI* variants.
	
	static void TexImage1D( /*something*/ );
	static void TexImage2D( TextureTarget a_TextureTarget, uint8_t a_MipMapLevel, TextureFormat a_InternalFormat, int32_t a_Width, int32_t a_Height, int32_t a_Border, TextureFormat a_TextureFormat, TextureSetting a_DataLayout, const void* a_Data );
	static void TexImage3D( /*something*/ );
	static void GenerateMipmap( TextureTarget a_TextureTarget );

	// Uniform access
	static int32_t GetUniformLocation( ShaderProgramHandle a_ShaderProgramHandle, const char* a_Name );
	static void Uniform1f( int32_t a_Location, float a_V0 );
	static void Uniform2f( int32_t a_Location, float a_V0, float a_V1 );
	static void Uniform3f( int32_t a_Location, float a_V0, float a_V1, float a_V2 );
	static void Uniform4f( int32_t a_Location, float a_V0, float a_V1, float a_V2, float a_V3 );
	static void Uniform1i( int32_t a_Location, int32_t a_V0 );
	static void Uniform2i( int32_t a_Location, int32_t a_V0, int32_t a_V1 );
	static void Uniform3i( int32_t a_Location, int32_t a_V0, int32_t a_V1, int32_t a_V2 );
	static void Uniform4i( int32_t a_Location, int32_t a_V0, int32_t a_V1, int32_t a_V2, int32_t a_V3 );
	static void Uniform1ui( int32_t a_Location, uint32_t a_V0 );
	static void Uniform2ui( int32_t a_Location, uint32_t a_V0, uint32_t a_V1 );
	static void Uniform3ui( int32_t a_Location, uint32_t a_V0, uint32_t a_V1, uint32_t a_V2 );
	static void Uniform4ui( int32_t a_Location, uint32_t a_V0, uint32_t a_V1, uint32_t a_V2, uint32_t a_V3 );
	static void Uniform1fv( int32_t a_Location, uint32_t a_Count, float* a_Value );
	static void Uniform2fv( int32_t a_Location, uint32_t a_Count, float* a_Value );
	static void Uniform3fv( int32_t a_Location, uint32_t a_Count, float* a_Value );
	static void Uniform4fv( int32_t a_Location, uint32_t a_Count, float* a_Value );
	static void Uniform1iv( int32_t a_Location, uint32_t a_Count, int32_t* a_Value );
	static void Uniform2iv( int32_t a_Location, uint32_t a_Count, int32_t* a_Value );
	static void Uniform3iv( int32_t a_Location, uint32_t a_Count, int32_t* a_Value );
	static void Uniform4iv( int32_t a_Location, uint32_t a_Count, int32_t* a_Value );
	static void Uniform1uiv( int32_t a_Location, uint32_t a_Count, uint32_t* a_Value );
	static void Uniform2uiv( int32_t a_Location, uint32_t a_Count, uint32_t* a_Value );
	static void Uniform3uiv( int32_t a_Location, uint32_t a_Count, uint32_t* a_Value );
	static void Uniform4uiv( int32_t a_Location, uint32_t a_Count, uint32_t* a_Value );
	static void UniformMatrix2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );
	static void UniformMatrix3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );
	static void UniformMatrix4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );
	static void UniformMatrix2x3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );
	static void UniformMatrix3x2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );
	static void UniformMatrix2x4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );
	static void UniformMatrix4x2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );
	static void UniformMatrix3x4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );
	static void UniformMatrix4x3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, float* a_Value );

	//----------SHADER_ACCESS--------------

private:

	template < typename _Type, Hash _Name >
	class UniformCommon
	{
	private:

		template < auto, typename, Hash > friend class Uniform;

		static _Type& Value()
		{
			static _Type Value;
			return Value;
		}

		static void Setup()
		{
			static bool Setup = []()
			{
				s_UniformMap[ _Name ] = &Value();
				return true;
			}();
		}
	};

	template < auto _Shader >
	class VaryingCommon
	{
	public:

		static uint32_t NextOffset( uint32_t a_size )
		{
			static uint32_t Offset = 0;
			uint32_t OriginalOffset = Offset;
			Offset += a_size;
			s_VaryingStrides[ ShaderAddress< _Shader > ] = Offset;
			return OriginalOffset;
		}

		static uint32_t NextIndex()
		{
			static uint32_t Index = 0;
			return Index++;
		}

		template < typename _Type, Hash _Name >
		static uint32_t Offset()
		{
			static uint32_t Offset = NextOffset( sizeof( _Type ) );
			return Offset;
		}

		template < typename _Type, Hash _Name >
		static uint32_t Index()
		{
			static uint32_t Index = NextIndex();
			return Index;
		}
	};

public:

	template < uint32_t _Location, typename _Type >
	class Attribute
	{
	public:

		static const _Type& Value()
		{
			static _Type Value;
			s_AttributeRegistry[ _Location ]( &Value );
			return Value;
		}
	};

	template < auto _Shader, typename _Type, Hash _Name >
	class Varying
	{
	public:

		static const _Type& In()
		{
			return *reinterpret_cast< _Type* >( reinterpret_cast< uint8_t* >( s_InterpolatedStorage.Data() ) + s_Offset );
		};

		static _Type& Out()
		{
			static OnStart Setup = s_Setup;
			return *reinterpret_cast< _Type* >( reinterpret_cast< uint8_t* >( s_VertexStorage.Head() ) + s_Offset );
		};

	private:

		static void Setup()
		{
			s_Offset = VaryingCommon< _Shader >::template Offset< _Type, _Name >();
		}

		inline static uint32_t s_Offset = 0;
		inline static OnStart s_Setup = Setup;
	};

	template < auto _Shader, typename _Type, Hash _Name >
	class Uniform
	{
	public:

		static const _Type& Value()
		{
			static OnStart Setup = s_Setup;
			return UniformCommon< _Type, _Name >::Value();
		}

	private:

		static void Setup()
		{
			UniformCommon< _Type, _Name >::Setup();
			s_UniformMap.Register( ShaderAddress< _Shader >, _Name );
		}

		inline static OnStart s_Setup = Setup;
	};

	template < typename _Type, Hash _Name >
	class InOut
	{
	public:

		static _Type& Value()
		{
			static _Type Value;
			return Value;
		}
	};

	template < typename _Type >
	static typename _Type::Output Sample( _Type a_Sampler, const typename _Type::Input& a_Input )
	{
		auto Handle = s_TextureUnits[ a_Sampler.Location ][ ( uint32_t )_Type::Target ];
		auto& Target = s_TextureRegistry[ Handle ];
		auto& Dimensions = Target.Dimensions;
		auto r = Vector2Int( ( Dimensions.x - 1 ) * a_Input.x, ( Dimensions.y - 1 ) * a_Input.y );
		uint32_t Position = r.y * Dimensions.x + r.x;
		const Colour* Data = reinterpret_cast< const Colour* >( Target.Data ) + Position;

		static constexpr float Denom = 1.0f / 255;
		return { Denom * Data->R, Denom * Data->G, Denom * Data->B, Denom * Data->A };
	}

private:

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
				m_ShaderPrograms[ a_Handle - 1 ].m_Shaders[ i ].Set = false;
				m_ShaderPrograms[ a_Handle - 1 ].m_Shaders[ i ].Handle = 0;
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

	//----------IMPLEMENTATION-------------

	class VertexAttribute
	{
	public:

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

		AttributeIterator( const VertexAttribute& a_VertexAttribute )
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

		void Clear()
		{
			m_Output = nullptr;
			m_Stride = 0;
			m_Data = nullptr;
		}

		AttributeIterator& operator=( const VertexAttribute& a_VertexAttribute )
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

		AttributeIterator& operator=( uint32_t a_Index )
		{
			m_Data = m_Begin + m_Stride * a_Index;
			return *this;
		}

		inline operator bool() const
		{
			return m_Data;
		}

		AttributeIterator& operator++()
		{
			m_Data += m_Stride;
			return *this;
		}

		AttributeIterator& operator+=( uint32_t a_Count )
		{
			m_Data += m_Stride * a_Count;
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

			Cast< GetDataType< static_cast< DataType >( Type ) >, Size + 1, Norm >( a_Data, o_Output, std::in_place_type< std::make_index_sequence< Size + 1 > > );
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
	class Texture
	{
	public:

		Texture()
			: TextureMinLOD( -1000 )
			, TextureMaxLOD( 1000 )
			, TextureMaxLevel( 1000 )
			, TextureSwizzleR( 0 )
			, TextureSwizzleG( 1 )
			, TextureSwizzleB( 2 )
			, TextureSwizzleA( 3 )
			, Data( nullptr )
			, Dimensions( 0 )
		{ }

		uint8_t     DepthStencilTextureMode : 1;
		int32_t     TextureBaseLevel;
		Vector4     TextureBorderColour;
		uint8_t     TextureCompareFunc      : 3;
		uint8_t     TextureCompareMode      : 1;
		float       TextureLODBias;
		int8_t      TextureMagFilter        : 1;
		int8_t      TextureMinFilter        : 3;
		float       TextureMinLOD;
		float       TextureMaxLOD;
		int32_t     TextureMaxLevel;
		int8_t      TextureSwizzleR         : 3;
		int8_t      TextureSwizzleG         : 3;
		int8_t      TextureSwizzleB         : 3;
		int8_t      TextureSwizzleA         : 3;
		uint8_t     TextureWrapS            : 3;
		uint8_t     TextureWrapT            : 3;
		uint8_t     TextureWrapR            : 3;
		const void* Data;
		Vector2Int  Dimensions;
		uint8_t     Format;
	};

	typedef std::vector< uint8_t >           Buffer;
	typedef std::array< VertexAttribute, 8 > Array;
	typedef std::map< void*, uint32_t >      StrideRegistry;
	typedef std::array< TextureHandle, 10  > TextureUnit;

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
	class TextureRegistry
	{
	public:

		TextureHandle Create()
		{
			size_t Index = 0;
			while ( m_Availability[ Index++ ] );
			m_Availability[ Index - 1 ] = true;
			m_Targets[ Index - 1 ] = -1;
			return Index;
		}

		bool Bind( TextureTarget a_Target, TextureHandle a_Handle )
		{
			if ( m_Targets[ a_Handle - 1 ] != -1 )
			{
				return false;
			}

			m_Targets[ a_Handle - 1 ] = ( int8_t )a_Target;
			return true;
		}

		void Destroy( TextureHandle a_Handle )
		{
			m_Availability[ a_Handle - 1 ] = false;
			// m_Textures[ a_Handle - 1 ] reset this
			m_Targets[ a_Handle - 1 ] = -1;
		}

		inline Texture& operator[]( BufferHandle a_Handle )
		{
			return m_Textures[ a_Handle - 1 ];
		}

		inline bool Valid( TextureHandle a_Handle )
		{
			return a_Handle - 1 < m_Availability.size() && m_Availability[ a_Handle - 1 ];
		}

	private:

		std::array< int8_t, 32 >  m_Targets;
		std::bitset< 32 >         m_Availability;
		std::array< Texture, 32 > m_Textures;
	};
	class AttributeRegistry
	{
	public:

		AttributeRegistry& operator++()
		{
			++m_VertexAttributes[ 0 ];
			++m_VertexAttributes[ 1 ];
			++m_VertexAttributes[ 2 ];
			++m_VertexAttributes[ 3 ];
			++m_VertexAttributes[ 4 ];
			++m_VertexAttributes[ 5 ];
			++m_VertexAttributes[ 6 ];
			++m_VertexAttributes[ 7 ];

			return *this;
		}

		AttributeRegistry& operator=( uint32_t a_Index )
		{
			m_VertexAttributes[ 0 ] = a_Index;
			m_VertexAttributes[ 1 ] = a_Index;
			m_VertexAttributes[ 2 ] = a_Index;
			m_VertexAttributes[ 3 ] = a_Index;
			m_VertexAttributes[ 4 ] = a_Index;
			m_VertexAttributes[ 5 ] = a_Index;
			m_VertexAttributes[ 6 ] = a_Index;
			m_VertexAttributes[ 7 ] = a_Index;

			return *this;
		}

		AttributeRegistry& operator+=( uint32_t a_Count )
		{
			m_VertexAttributes[ 0 ] += a_Count;
			m_VertexAttributes[ 1 ] += a_Count;
			m_VertexAttributes[ 2 ] += a_Count;
			m_VertexAttributes[ 3 ] += a_Count;
			m_VertexAttributes[ 4 ] += a_Count;
			m_VertexAttributes[ 5 ] += a_Count;
			m_VertexAttributes[ 6 ] += a_Count;
			m_VertexAttributes[ 7 ] += a_Count;
		}

		template < typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > > >
		AttributeRegistry& operator=( T* a_Indices )
		{
			m_VertexAttributes[ 0 ] = static_cast< uint32_t >( a_Indices[ 0 ] );
			m_VertexAttributes[ 1 ] = static_cast< uint32_t >( a_Indices[ 1 ] );
			m_VertexAttributes[ 2 ] = static_cast< uint32_t >( a_Indices[ 2 ] );
			m_VertexAttributes[ 3 ] = static_cast< uint32_t >( a_Indices[ 3 ] );
			m_VertexAttributes[ 4 ] = static_cast< uint32_t >( a_Indices[ 4 ] );
			m_VertexAttributes[ 5 ] = static_cast< uint32_t >( a_Indices[ 5 ] );
			m_VertexAttributes[ 6 ] = static_cast< uint32_t >( a_Indices[ 6 ] );
			m_VertexAttributes[ 7 ] = static_cast< uint32_t >( a_Indices[ 7 ] );
		}

		inline void Reset()
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

		inline AttributeIterator& operator[]( uint32_t a_Position )
		{
			return m_VertexAttributes[ a_Position ];
		}

	private:

		AttributeIterator m_VertexAttributes[ 8 ];
	};
	class UniformMap
	{
	public:

		inline void*& operator[]( Hash a_UniformName )
		{
			return m_Uniforms[ a_UniformName ];
		}

		inline const std::vector< std::pair< Hash, void* > >& operator[]( void* a_Shader )
		{
			return m_ShaderLookup[ a_Shader ];
		}

		void Register( void* a_Shader, Hash a_UniformName )
		{
			auto& Entry = m_ShaderLookup[ a_Shader ];
			Entry.emplace_back( a_UniformName, ( *this )[ a_UniformName ] );
		}

	private:

		typedef std::map< void*, std::vector< std::pair< Hash, void* > > > ShaderLookup;
		typedef std::map< Hash, void* > UniformArray;

		ShaderLookup m_ShaderLookup;
		UniformArray m_Uniforms;
	};
	
	//remove later
	class AttribVector
	{
	public:

		AttribVector() = default;

		AttribVector( uint32_t a_Size, float* a_Data = nullptr )
		{
			m_Data.resize( a_Size );

			if ( !a_Data )
			{
				return;
			}

			for ( uint32_t i = 0; i < a_Size; ++i )
			{
				m_Data[ i ] = a_Data[ i ];
			}
		}

		AttribVector( const AttribVector& a_Vector )
		{
			m_Data.resize( a_Vector.m_Data.size() );

			for ( uint32_t i = 0; i < m_Data.size(); ++i )
			{
				m_Data[ i ] = a_Vector.m_Data[ i ];
			}
		}

		AttribVector( AttribVector&& a_Vector )
		{
			m_Data = std::move( a_Vector.m_Data );
		}

		AttribVector& operator=( const AttribVector& a_Vector )
		{
			m_Data.resize( a_Vector.m_Data.size() );

			for ( uint32_t i = 0; i < m_Data.size(); ++i )
			{
				m_Data[ i ] = a_Vector.m_Data[ i ];
			}

			return *this;
		}

		AttribVector& operator+=( const AttribVector& a_Vector )
		{
			for ( uint32_t i = 0; i < m_Data.size(); ++i )
			{
				m_Data[ i ] += a_Vector.m_Data[ i ];
			}

			return *this;
		}

		AttribVector& operator-=( const AttribVector& a_Vector )
		{
			for ( uint32_t i = 0; i < m_Data.size(); ++i )
			{
				m_Data[ i ] -= a_Vector.m_Data[ i ];
			}

			return *this;
		}

		AttribVector& operator*=( const AttribVector& a_Vector )
		{
			for ( uint32_t i = 0; i < m_Data.size(); ++i )
			{
				m_Data[ i ] *= a_Vector.m_Data[ i ];
			}

			return *this;
		}

		AttribVector& operator/=( const AttribVector& a_Vector )
		{
			for ( uint32_t i = 0; i < m_Data.size(); ++i )
			{
				m_Data[ i ] /= a_Vector.m_Data[ i ];
			}

			return *this;
		}

		AttribVector& operator*=( float a_Scalar )
		{
			for ( uint32_t i = 0; i < m_Data.size(); ++i )
			{
				m_Data[ i ] *= a_Scalar;
			}

			return *this;
		}

		AttribVector& operator/=( float a_Scalar )
		{
			a_Scalar = 1.0f / a_Scalar;

			for ( uint32_t i = 0; i < m_Data.size(); ++i )
			{
				m_Data[ i ] *= a_Scalar;
			}

			return *this;
		}

		float& operator[]( uint32_t a_Index )
		{
			return m_Data[ a_Index ];
		}

		inline float* Data()
		{
			return m_Data.data();
		}

		inline size_t Size() const
		{
			return m_Data.size();
		}

		inline void Resize( size_t a_Size )
		{
			m_Data.resize( a_Size );
		}

	private:

		std::vector< float > m_Data;
	};
	
	template < typename T = uint8_t >
	class DataStorage
	{
	public:

		DataStorage()
			: m_Head( nullptr )
			, m_Stride( sizeof( T ) )
		{ }

		template < typename U >
		DataStorage( const DataStorage< U >& a_DataStorage )
		{
			m_Data   = a_DataStorage.m_Data;
			m_Head   = m_Data.data();
			m_Stride = a_DataStorage.m_Stride;
		}

		template < typename U >
		DataStorage( DataStorage< U >&& a_DataStorage )
			: m_Data( std::move( a_DataStorage.m_Data ) )
			, m_Head( a_DataStorage.m_Head )
			, m_Stride( a_DataStorage.m_Stride )
		{ }

		DataStorage( size_t a_Size, size_t a_Stride = sizeof( T ) )
		{
			Prepare( a_Size, a_Stride );
		}

		void Prepare( size_t a_Size, size_t a_Stride = sizeof( T ) )
		{
			if ( m_Data.size() < a_Size * a_Stride )
			{
				m_Data.resize( a_Size * a_Stride );
			}

			m_Head = m_Data.data();
			m_Stride = a_Stride;
		}

		inline DataStorage& operator++()
		{
			m_Head += m_Stride;
			return *this;
		}

		DataStorage operator++( int )
		{
			DataStorage Temp( *this );
			++*this;
			return Temp;
		}

		inline DataStorage& operator=( const T& a_Value )
		{
			*Head() = a_Value;
			return *this;
		}

		inline void Reset()
		{
			m_Head = m_Data.data();
		}

		inline uint8_t* Raw()
		{
			return m_Data.data();
		}

		inline const uint8_t* Raw() const
		{
			return m_Data.data();
		}

		T* Data()
		{
			return reinterpret_cast< T* >( m_Data.data() );
		}

		const T* Data() const
		{
			return reinterpret_cast< T* >( m_Data.data() );
		}

		T* Head()
		{
			return reinterpret_cast< T* >( m_Head );
		}

		const T* Head() const
		{
			return reinterpret_cast< T* >( m_Head );
		}

		inline operator T& ()
		{
			return *Head();
		}

		inline operator T& const () const
		{
			return *Head();
		}

		void Clear()
		{
			m_Data.clear();
			m_Head = nullptr;
			m_Stride = 0;
		}

	private:

		std::vector< uint8_t > m_Data;
		uint8_t*               m_Head;
		size_t                 m_Stride;
	};
	
	template < typename T >
	class AttribSpan
	{
	public:

		AttribSpan()
			: m_Origin( nullptr )
			, m_Size( 0 )
		{ }

		AttribSpan( T* a_Origin, size_t a_Size )
			: m_Origin( a_Origin )
			, m_Size( a_Size )
		{ }

		AttribSpan( AttribSpan&& a_AttribSpan )
			: m_Origin( a_AttribSpan.m_Origin )
			, m_Size( a_AttribSpan.m_Size )
		{ }

		AttribSpan& operator=( const AttribSpan& a_AttribSpan )
		{
			for ( uint32_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] = a_AttribSpan[ i ];
			}

			return *this;
		}

		T& operator[]( size_t a_Index )
		{
			return m_Origin[ a_Index ];
		}

		const T& operator[]( size_t a_Index ) const
		{
			return m_Origin[ a_Index ];
		}

		AttribSpan& operator+=( const AttribSpan& a_AttribSpan )
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] += a_AttribSpan[ i ];
			}

			return *this;
		}

		AttribSpan& operator-=( const AttribSpan& a_AttribSpan )
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] -= a_AttribSpan[ i ];
			}

			return *this;
		}

		AttribSpan& operator*=( const AttribSpan& a_AttribSpan )
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] *= a_AttribSpan[ i ];
			}

			return *this;
		}

		AttribSpan& operator/=( const AttribSpan& a_AttribSpan )
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] /= a_AttribSpan[ i ];
			}

			return *this;
		}

		AttribSpan& operator+=( T a_Value )
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] += a_Value;
			}

			return *this;
		}

		AttribSpan& operator-=( T a_Value )
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] -= a_Value;
			}

			return *this;
		}

		AttribSpan& operator*=( T a_Value )
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] *= a_Value;
			}

			return *this;
		}

		AttribSpan& operator/=( T a_Value )
		{
			for ( size_t i = 0; i < m_Size; ++i )
			{
				m_Origin[ i ] /= a_Value;
			}

			return *this;
		}

		T& operator*()
		{
			return *m_Origin;
		}

		const T& operator*() const
		{
			return *m_Origin;
		}

		T* operator->()
		{
			return m_Origin;
		}

		const T* operator->() const
		{
			return m_Origin;
		}

		inline AttribSpan& Advance()
		{
			m_Origin += m_Size;
			return *this;
		}

		inline AttribSpan& Advance( uint32_t a_Count )
		{
			m_Origin += m_Size * a_Count;
			return *this;
		}

		inline void Set( T* a_Origin, size_t a_Size )
		{
			m_Origin = a_Origin;
			m_Size = a_Size;
		}

		inline void Swap( AttribSpan& a_AttribSpan )
		{
			/*std::swap( m_Origin, a_AttribSpan.m_Origin );
			std::swap( m_Size, a_AttribSpan.m_Size );*/

			for ( uint32_t i = 0; i < a_AttribSpan.m_Size; ++i )
			{
				std::swap( m_Origin[ i ], a_AttribSpan.m_Origin[ i ] );
			}
		}

	private:

		T*     m_Origin;
		size_t m_Size;
	};
	
	class RenderState
	{
	public:

		RenderState()
			: BackCull( true )
		{ }

		bool AlphaBlend    : 1;
		bool CullFace      : 1;
		bool FrontCull     : 1;
		bool BackCull      : 1;
		bool DepthTest     : 1;
		bool Clip          : 1;
	};
	class DepthBuffer
	{
	public:

		DepthBuffer()
			: m_Buffer( nullptr )
		{ }

		~DepthBuffer()
		{
			delete[] m_Buffer;
		}

		void Init( Vector2Int a_Size )
		{
			m_Size = a_Size;
			m_Buffer = new float[ a_Size.x * a_Size.y ];
		}

		inline bool Test( uint32_t a_X, uint32_t a_Y, float a_W )
		{
			return m_Buffer[ a_Y * m_Size.x + a_X ] > a_W;
		}

		void Commit( uint32_t a_X, uint32_t a_Y, float a_W )
		{
			m_Buffer[ a_Y * m_Size.x + a_X ] = a_W;
		}

		bool TestAndCommit( uint32_t a_X, uint32_t a_Y, float a_W )
		{
			float& Point = m_Buffer[ a_Y * m_Size.x + a_X ];
			
			if ( Point > a_W )
			{
				Point = a_W;
				return true;
			}

			return false;
		}

		void Reset( float a_Depth )
		{
			float *Begin = m_Buffer, *End = m_Buffer + ( m_Size.x * m_Size.y );

			for ( ; Begin != End; ++Begin )
			{
				*Begin = a_Depth;
			}
		}

	private:

		Vector2Int m_Size;
		float*     m_Buffer;
	};

	static void ProcessVertices( uint32_t a_Begin, uint32_t a_End, uint32_t a_Stride, void( *a_VertexShader )() )
	{
		s_VertexStorage.Prepare( a_End - a_Begin, a_Stride * sizeof( float ) );
		s_PositionStorage.Prepare( a_End - a_Begin );
		s_AttributeRegistry = a_Begin;
		AttribSpan< float > AttribView( s_VertexStorage.Data(), a_Stride );
		Vector2 HalfWindow( ConsoleWindow::GetCurrentContext()->GetWidth(), ConsoleWindow::GetCurrentContext()->GetHeight() );
		HalfWindow *= 0.5f;

		for ( ; a_Begin < a_End; ++a_Begin )
		{
			a_VertexShader();

			Position.w = 1.0f / Position.w;
			Position.x *= Position.w;
			Position.y *= Position.w;
			Position.z *= Position.w;
			AttribView *= Position.w;
			Position.x *= HalfWindow.x;
			Position.y *= HalfWindow.y;
			Position.x += HalfWindow.x;
			Position.y += HalfWindow.y;
			s_PositionStorage = Position;

			++s_AttributeRegistry;
			++s_VertexStorage;
			++s_PositionStorage;
			AttribView.Advance();
		}
	}

	static void ProcessFragments( uint32_t a_Begin, uint32_t a_End, uint32_t a_Stride, void( *a_FragmentShader )() )
	{
		static AttribSpan< Vector4 > P[ 3 ];
		static AttribSpan< float   > V[ 3 ];

		auto& ActiveArray = s_ArrayRegistry[ s_ActiveArray ];
		Rect ViewPort = ConsoleWindow::GetCurrentContext()->GetScreenBuffer().GetBufferRect();

		// Reset position and vertex storage.
		s_VertexStorage.Reset();
		s_PositionStorage.Reset();
		s_InterpolatedStorage.Prepare( a_Stride );

		P[ 0 ].Set( s_PositionStorage.Head() + 0ul, 1 );
		P[ 1 ].Set( s_PositionStorage.Head() + 1ul, 1 );
		P[ 2 ].Set( s_PositionStorage.Head() + 2ul, 1 );
		V[ 0 ].Set( s_VertexStorage  .Head() + 0ul * a_Stride, a_Stride );
		V[ 1 ].Set( s_VertexStorage  .Head() + 1ul * a_Stride, a_Stride );
		V[ 2 ].Set( s_VertexStorage  .Head() + 2ul * a_Stride, a_Stride );

		for ( ; a_Begin < a_End; a_Begin += 3
			  , P[ 0 ].Advance( 3 )
			  , P[ 1 ].Advance( 3 )
			  , P[ 2 ].Advance( 3 )
			  , V[ 0 ].Advance( 3 )
			  , V[ 1 ].Advance( 3 )
			  , V[ 2 ].Advance( 3 ) )
		{
			// Clipping - Improve later - Currently reject if any point outside viewport.
			bool Reject = false;

			for ( uint32_t i = 0; i < 3; ++i )
			{
				if ( !ViewPort.Contains( *P[ i ] ) )
				{
					Reject = true;
					break;
				}
			}

			if ( Reject )
			{
				continue;
			}

			// Culling - Improve later - if enabled and correct orientation, continue.
			if ( s_RenderState.CullFace )
			{
				float NormalZ = Math::Cross( Vector3( *P[ 1 ] - *P[ 0 ] ), Vector3( *P[ 2 ] - *P[ 0 ] ) ).z;

				if ( s_RenderState.FrontCull && NormalZ < 0.0f )
				{
					continue;
				}

				if ( s_RenderState.BackCull && NormalZ > 0.0f )
				{
					continue;
				}
			}

			// Correct Y
			P[ 0 ]->y = -P[ 0 ]->y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();
			P[ 1 ]->y = -P[ 1 ]->y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();
			P[ 2 ]->y = -P[ 2 ]->y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();

			// Sort corners.
			if ( P[ 0 ]->y < P[ 1 ]->y )
			{
				P[ 0 ].Swap( P[ 1 ] );
				V[ 0 ].Swap( V[ 1 ] );
			}

			if ( P[ 0 ]->y < P[ 2 ]->y )
			{
				P[ 0 ].Swap( P[ 2 ] );
				V[ 0 ].Swap( V[ 2 ] );
			}

			if ( P[ 1 ]->y < P[ 2 ]->y )
			{
				P[ 1 ].Swap( P[ 2 ] );
				V[ 1 ].Swap( V[ 2 ] );
			}

			// Align y.
			P[ 0 ]->y = static_cast< int >( P[ 0 ]->y );
			P[ 1 ]->y = static_cast< int >( P[ 1 ]->y );
			P[ 2 ]->y = static_cast< int >( P[ 2 ]->y );

			if ( P[ 0 ]->y == P[ 2 ]->y )
			{
				continue;
			}

			// Setup Position and Attribute values.
			float SpanX, SpanY, Y;
			static DataStorage< Vector4 > Positions;
			static AttribSpan < Vector4 > PMid, PStep, PStepL, PStepR, PBegin, PL, PR; // 7
			static DataStorage< float >   Attributes;
			static AttribSpan < float >   VMid, VStep, VStepL, VStepR, VBegin, VL, VR; // 7
			static AttribSpan < float >   InterpolatedValues;

			Positions.Prepare( 7 );
			Attributes.Prepare( 7, a_Stride * sizeof( float ) ); 
			InterpolatedValues.Set( s_InterpolatedStorage.Data(), a_Stride );

			PMid  .Set( Positions .Head() + 0ul, 1                   );
			PStep .Set( Positions .Head() + 1ul, 1                   );
			PStepL.Set( Positions .Head() + 2ul, 1                   );
			PStepR.Set( Positions .Head() + 3ul, 1                   );
			PBegin.Set( Positions .Head() + 4ul, 1                   );
			PL    .Set( Positions .Head() + 5ul, 1                   );
			PR    .Set( Positions .Head() + 6ul, 1                   );
			VMid  .Set( Attributes.Head() + 0ul * a_Stride, a_Stride );
			VStep .Set( Attributes.Head() + 1ul * a_Stride, a_Stride );
			VStepL.Set( Attributes.Head() + 2ul * a_Stride, a_Stride );
			VStepR.Set( Attributes.Head() + 3ul * a_Stride, a_Stride );
			VBegin.Set( Attributes.Head() + 4ul * a_Stride, a_Stride );
			VL    .Set( Attributes.Head() + 5ul * a_Stride, a_Stride );
			VR    .Set( Attributes.Head() + 6ul * a_Stride, a_Stride );

			// Find Mid point in screen space.
			float L = ( P[ 1 ]->y - P[ 2 ]->y ) / ( P[ 0 ]->y - P[ 2 ]->y );
			*PMid = Math::Lerp( L, *P[ 2 ], *P[ 0 ] );
			PMid->y = static_cast< int >( PMid->y );

			// Calculate Vertex attributes mid point.
			for ( uint32_t i = 0; i < a_Stride; ++i )
			{
				VMid[ i ] = Math::Lerp( L, V[ 2 ][ i ], V[ 0 ][ i ] );
			}

			// Swap  vertex 1 and Mid if Mid is not on right.
			if ( PMid->x < P[ 1 ]->x )
			{
				PMid.Swap( P[ 1 ] );
				VMid.Swap( V[ 1 ] );
			}

			if ( P[ 2 ]->y != P[ 1 ]->y )
			{
				SpanY = 1.0f / ( P[ 1 ]->y - P[ 2 ]->y );
				*PStepL = *P[ 1 ] - *P[ 2 ];
				*PStepL *= SpanY;
				*PStepR = *PMid - *P[ 2 ];
				*PStepR *= SpanY;
				*PL = *P[ 2 ];
				*PR = *P[ 2 ];
				VStepL = V[ 1 ];
				VStepL -= V[ 2 ];
				VStepL *= SpanY;
				VStepR = VMid;
				VStepR -= V[ 2 ];
				VStepR *= SpanY;
				VL = V[ 2 ];
				VR = V[ 2 ];
				Y = P[ 2 ]->y;

				for ( ; Y < P[ 1 ]->y; ++Y )
				{
					*PL += *PStepL;
					*PR += *PStepR;
					VL += VStepL;
					VR += VStepR;
					SpanX = 1.0f / ( PR->x - PL->x );
					*PBegin = *PL;
					*PStep = ( *PR - *PL ) * SpanX;
					VBegin = VL;
					VStep  = VR;
					VStep -= VL;
					VStep *= SpanX;

					for ( ; PBegin->x < static_cast< int >( PR->x ); *PBegin += *PStep, VBegin += VStep )
					{
						if ( s_RenderState.DepthTest && !s_DepthBuffer.TestAndCommit( PBegin->x, PBegin->y, PBegin->z / PBegin->w ) )
						{
							continue;
						}

						InterpolatedValues = VBegin;
						InterpolatedValues /= PBegin->w;
						//_FragCoord = PBegin;
						a_FragmentShader();
						Colour Fragment( 255 * FragColour[ 0 ], 255 * FragColour[ 1 ], 255 * FragColour[ 2 ], 255 * FragColour[ 3 ] );
						ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetColour( { PBegin->x, Y }, Fragment );
					}
				}
			}
			else
			{
				Y = P[ 1 ]->y;
				*PL = *P[ 1 ];
				*PR = *PMid;
				VL = V[ 1 ];
				VR = VMid;
			}

			if ( P[ 1 ]->y != P[ 0 ]->y )
			{
				SpanY = 1.0f / ( P[ 0 ]->y - P[ 1 ]->y );
				*PStepL = *P[ 0 ] - *P[ 1 ]; 
				*PStepL *= SpanY;
				*PStepR = *P[ 0 ] - *PMid; 
				*PStepR *= SpanY;
				VStepL = V[ 0 ];
				VStepL -= V[ 1 ];
				VStepL *= SpanY;
				VStepR = V[ 0 ];
				VStepR -= VMid;
				VStepR *= SpanY;

				for ( ; Y < P[ 0 ]->y; ++Y )
				{
					*PL += *PStepL;
					*PR += *PStepR;
					VL += VStepL;
					VR += VStepR;
					SpanX = 1.0f / ( PR->x - PL->x );
					*PBegin = *PL;
					*PStep = ( *PR - *PL ) * SpanX;
					VBegin = VL;
					VStep = VR;
					VStep -= VL;
					VStep *= SpanX;

					for ( ; PBegin->x < static_cast< int >( PR->x ); *PBegin += *PStep, VBegin += VStep )
					{

						if ( s_RenderState.DepthTest && !s_DepthBuffer.TestAndCommit( PBegin->x, PBegin->y, PBegin->z / PBegin->w ) )
						{
							continue;
						}

						InterpolatedValues = VBegin;
						InterpolatedValues /= PBegin->w;
						//_FragCoord = PBegin;
						a_FragmentShader();
						Colour Fragment( 255 * FragColour[ 0 ], 255 * FragColour[ 1 ], 255 * FragColour[ 2 ], 255 * FragColour[ 3 ] );
						ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetColour( { PBegin->x, Y }, Fragment );
					}
				}
			}
		}
	}

	template < uint8_t _Interface >
	static void DrawArraysImpl( uint32_t a_Begin, uint32_t a_Count )
	{
		auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
		uint32_t AttribStride = s_VaryingStrides[ ActiveProgram[ ShaderType::VERTEX_SHADER ] ] / sizeof( float );

		ProcessVertices ( a_Begin, a_Begin + a_Count, AttribStride, ActiveProgram[ ShaderType::VERTEX_SHADER   ] );
		ProcessFragments( a_Begin, a_Begin + a_Count, AttribStride, ActiveProgram[ ShaderType::FRAGMENT_SHADER ] );
	}

	template < typename T >
	static void TexParameterImpl( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, T a_Value )
	{
		TextureHandle Handle = s_TextureUnits[ s_ActiveTextureUnit ][ ( uint32_t )a_TextureTarget ];
		Texture& Target = s_TextureRegistry[ Handle ];

		switch ( a_TextureParameter )
		{
			case TextureParameter::DEPTH_STENCIL_TEXTURE_MODE:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::DEPTH_COMPONENT: Target.DepthStencilTextureMode = 0; break;
					case ( uint32_t )TextureSetting::STENCIL_INDEX:   Target.DepthStencilTextureMode = 1; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_BASE_LEVEL:
				if constexpr ( !std::is_pointer_v< T > )
				Target.TextureBaseLevel = a_Value; break;
			case TextureParameter::TEXTURE_BORDER_COLOUR:
			{
				if constexpr ( std::is_pointer_v< T > ) {
				const float* Value = reinterpret_cast< const float* >( a_Value );
				Target.TextureBorderColour = {
					Value[ 0 ],
					Value[ 1 ],
					Value[ 2 ],
					Value[ 3 ]
				}; }
				break;
			}
			case TextureParameter::TEXTURE_COMPARE_FUNC:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::LEQUAL:    Target.TextureCompareFunc = 0; break;
					case ( uint32_t )TextureSetting::GEQUAL:    Target.TextureCompareFunc = 1; break;
					case ( uint32_t )TextureSetting::LESS:      Target.TextureCompareFunc = 2; break;
					case ( uint32_t )TextureSetting::GREATER:   Target.TextureCompareFunc = 3; break;
					case ( uint32_t )TextureSetting::EQUAL:     Target.TextureCompareFunc = 4; break;
					case ( uint32_t )TextureSetting::NOT_EQUAL: Target.TextureCompareFunc = 5; break;
					case ( uint32_t )TextureSetting::ALWAYS:    Target.TextureCompareFunc = 6; break;
					case ( uint32_t )TextureSetting::NEVER:     Target.TextureCompareFunc = 7; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_COMPARE_MODE:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::NONE:                   Target.TextureCompareMode = 0; break;
					case ( uint32_t )TextureSetting::COMPARE_REF_TO_TEXTURE: Target.TextureCompareMode = 1; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_LOD_BIAS:
				if constexpr ( !std::is_pointer_v< T > )
				Target.TextureLODBias = a_Value; break;
			case TextureParameter::TEXTURE_MIN_FILTER:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::NEAREST:                Target.TextureMinFilter = 0; break;
					case ( uint32_t )TextureSetting::LINEAR:                 Target.TextureMinFilter = 1; break;
					case ( uint32_t )TextureSetting::NEAREST_MIPMAP_NEAREST: Target.TextureMinFilter = 2; break;
					case ( uint32_t )TextureSetting::LINEAR_MIPMAP_NEAREST:  Target.TextureMinFilter = 3; break;
					case ( uint32_t )TextureSetting::NEAREST_MIPMAP_LINEAR:  Target.TextureMinFilter = 4; break;
					case ( uint32_t )TextureSetting::LINEAR_MIPMAP_LINEAR:   Target.TextureMinFilter = 5; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_MAG_FILTER:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::NEAREST: Target.TextureMinFilter = 0; break;
					case ( uint32_t )TextureSetting::LINEAR:  Target.TextureMinFilter = 1; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_MIN_LOD:
				if constexpr ( !std::is_pointer_v< T > )
				Target.TextureMinLOD = a_Value; break;
			case TextureParameter::TEXTURE_MAX_LOD:
				if constexpr ( !std::is_pointer_v< T > )
				Target.TextureMaxLOD = a_Value; break;
			case TextureParameter::TEXTURE_MAX_LEVEL:
				if constexpr ( !std::is_pointer_v< T > )
				Target.TextureMaxLevel = a_Value; break;
			case TextureParameter::TEXTURE_SWIZZLE_R:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleR = 0; break;
					case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleR = 1; break;
					case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleR = 2; break;
					case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleR = 3; break;
					case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleR = 4; break;
					case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleR = 5; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_SWIZZLE_G:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleG = 0; break;
					case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleG = 1; break;
					case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleG = 2; break;
					case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleG = 3; break;
					case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleG = 4; break;
					case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleG = 5; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_SWIZZLE_B:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleB = 0; break;
					case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleB = 1; break;
					case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleB = 2; break;
					case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleB = 3; break;
					case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleB = 4; break;
					case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleB = 5; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_SWIZZLE_A:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleA = 0; break;
					case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleA = 1; break;
					case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleA = 2; break;
					case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleA = 3; break;
					case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleA = 4; break;
					case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleA = 5; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_SWIZZLE_RGBA:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleR = 0; Target.TextureSwizzleG = 0; Target.TextureSwizzleB = 0; Target.TextureSwizzleA = 0; break;
					case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleR = 1; Target.TextureSwizzleG = 1; Target.TextureSwizzleB = 1; Target.TextureSwizzleA = 1; break;
					case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleR = 2; Target.TextureSwizzleG = 2; Target.TextureSwizzleB = 2; Target.TextureSwizzleA = 2; break;
					case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleR = 3; Target.TextureSwizzleG = 3; Target.TextureSwizzleB = 3; Target.TextureSwizzleA = 3; break;
					case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleR = 4; Target.TextureSwizzleG = 4; Target.TextureSwizzleB = 4; Target.TextureSwizzleA = 4; break;
					case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleR = 5; Target.TextureSwizzleG = 5; Target.TextureSwizzleB = 5; Target.TextureSwizzleA = 5; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_WRAP_S:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::CLAMP_TO_EDGE:        Target.TextureWrapS = 0; break;
					case ( uint32_t )TextureSetting::CLAMP_TO_BORDER:      Target.TextureWrapS = 1; break;
					case ( uint32_t )TextureSetting::MIRRORED_REPEAT:      Target.TextureWrapS = 2; break;
					case ( uint32_t )TextureSetting::REPEAT:               Target.TextureWrapS = 3; break;
					case ( uint32_t )TextureSetting::MIRROR_CLAMP_TO_EDGE: Target.TextureWrapS = 4; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_WRAP_T:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::CLAMP_TO_EDGE:        Target.TextureWrapT = 0; break;
					case ( uint32_t )TextureSetting::CLAMP_TO_BORDER:      Target.TextureWrapT = 1; break;
					case ( uint32_t )TextureSetting::MIRRORED_REPEAT:      Target.TextureWrapT = 2; break;
					case ( uint32_t )TextureSetting::REPEAT:               Target.TextureWrapT = 3; break;
					case ( uint32_t )TextureSetting::MIRROR_CLAMP_TO_EDGE: Target.TextureWrapT = 4; break;
					default: break;
				}
				break;
			}
			case TextureParameter::TEXTURE_WRAP_R:
			{
				if constexpr ( std::is_integral_v< T > )
				switch ( a_Value )
				{
					case ( uint32_t )TextureSetting::CLAMP_TO_EDGE:        Target.TextureWrapR = 0; break;
					case ( uint32_t )TextureSetting::CLAMP_TO_BORDER:      Target.TextureWrapR = 1; break;
					case ( uint32_t )TextureSetting::MIRRORED_REPEAT:      Target.TextureWrapR = 2; break;
					case ( uint32_t )TextureSetting::REPEAT:               Target.TextureWrapR = 3; break;
					case ( uint32_t )TextureSetting::MIRROR_CLAMP_TO_EDGE: Target.TextureWrapR = 4; break;
					default: break;
				}
				break;
			}
			default:
				break;
		}
	}

	template < typename T >
	static void TextureParameterImpl( TextureHandle a_Handle, TextureParameter a_TextureParameter, T a_Value )
	{
		Texture& Target = s_TextureRegistry[ a_Handle ];

		switch ( a_TextureParameter )
		{
			case TextureParameter::DEPTH_STENCIL_TEXTURE_MODE:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::DEPTH_COMPONENT: Target.DepthStencilTextureMode = 0; break;
						case ( uint32_t )TextureSetting::STENCIL_INDEX:   Target.DepthStencilTextureMode = 1; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_BASE_LEVEL:
				if constexpr ( !std::is_pointer_v< T > )
					Target.TextureBaseLevel = a_Value; break;
			case TextureParameter::TEXTURE_BORDER_COLOUR:
			{
				if constexpr ( std::is_pointer_v< T > )
				{
					const float* Value = reinterpret_cast< const float* >( a_Value );
					Target.TextureBorderColour = {
						Value[ 0 ],
						Value[ 1 ],
						Value[ 2 ],
						Value[ 3 ]
					};
				}
				break;
			}
			case TextureParameter::TEXTURE_COMPARE_FUNC:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::LEQUAL:    Target.TextureCompareFunc = 0; break;
						case ( uint32_t )TextureSetting::GEQUAL:    Target.TextureCompareFunc = 1; break;
						case ( uint32_t )TextureSetting::LESS:      Target.TextureCompareFunc = 2; break;
						case ( uint32_t )TextureSetting::GREATER:   Target.TextureCompareFunc = 3; break;
						case ( uint32_t )TextureSetting::EQUAL:     Target.TextureCompareFunc = 4; break;
						case ( uint32_t )TextureSetting::NOT_EQUAL: Target.TextureCompareFunc = 5; break;
						case ( uint32_t )TextureSetting::ALWAYS:    Target.TextureCompareFunc = 6; break;
						case ( uint32_t )TextureSetting::NEVER:     Target.TextureCompareFunc = 7; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_COMPARE_MODE:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::NONE:                   Target.TextureCompareMode = 0; break;
						case ( uint32_t )TextureSetting::COMPARE_REF_TO_TEXTURE: Target.TextureCompareMode = 1; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_LOD_BIAS:
				if constexpr ( !std::is_pointer_v< T > )
					Target.TextureLODBias = a_Value; break;
			case TextureParameter::TEXTURE_MIN_FILTER:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::NEAREST:                Target.TextureMinFilter = 0; break;
						case ( uint32_t )TextureSetting::LINEAR:                 Target.TextureMinFilter = 1; break;
						case ( uint32_t )TextureSetting::NEAREST_MIPMAP_NEAREST: Target.TextureMinFilter = 2; break;
						case ( uint32_t )TextureSetting::LINEAR_MIPMAP_NEAREST:  Target.TextureMinFilter = 3; break;
						case ( uint32_t )TextureSetting::NEAREST_MIPMAP_LINEAR:  Target.TextureMinFilter = 4; break;
						case ( uint32_t )TextureSetting::LINEAR_MIPMAP_LINEAR:   Target.TextureMinFilter = 5; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_MAG_FILTER:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::NEAREST: Target.TextureMinFilter = 0; break;
						case ( uint32_t )TextureSetting::LINEAR:  Target.TextureMinFilter = 1; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_MIN_LOD:
				if constexpr ( !std::is_pointer_v< T > )
					Target.TextureMinLOD = a_Value; break;
			case TextureParameter::TEXTURE_MAX_LOD:
				if constexpr ( !std::is_pointer_v< T > )
					Target.TextureMaxLOD = a_Value; break;
			case TextureParameter::TEXTURE_MAX_LEVEL:
				if constexpr ( !std::is_pointer_v< T > )
					Target.TextureMaxLevel = a_Value; break;
			case TextureParameter::TEXTURE_SWIZZLE_R:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleR = 0; break;
						case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleR = 1; break;
						case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleR = 2; break;
						case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleR = 3; break;
						case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleR = 4; break;
						case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleR = 5; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_SWIZZLE_G:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleG = 0; break;
						case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleG = 1; break;
						case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleG = 2; break;
						case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleG = 3; break;
						case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleG = 4; break;
						case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleG = 5; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_SWIZZLE_B:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleB = 0; break;
						case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleB = 1; break;
						case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleB = 2; break;
						case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleB = 3; break;
						case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleB = 4; break;
						case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleB = 5; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_SWIZZLE_A:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleA = 0; break;
						case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleA = 1; break;
						case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleA = 2; break;
						case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleA = 3; break;
						case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleA = 4; break;
						case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleA = 5; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_SWIZZLE_RGBA:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::RED:   Target.TextureSwizzleR = 0; Target.TextureSwizzleG = 0; Target.TextureSwizzleB = 0; Target.TextureSwizzleA = 0; break;
						case ( uint32_t )TextureSetting::GREEN: Target.TextureSwizzleR = 1; Target.TextureSwizzleG = 1; Target.TextureSwizzleB = 1; Target.TextureSwizzleA = 1; break;
						case ( uint32_t )TextureSetting::BLUE:  Target.TextureSwizzleR = 2; Target.TextureSwizzleG = 2; Target.TextureSwizzleB = 2; Target.TextureSwizzleA = 2; break;
						case ( uint32_t )TextureSetting::ALPHA: Target.TextureSwizzleR = 3; Target.TextureSwizzleG = 3; Target.TextureSwizzleB = 3; Target.TextureSwizzleA = 3; break;
						case ( uint32_t )TextureSetting::ZERO:  Target.TextureSwizzleR = 4; Target.TextureSwizzleG = 4; Target.TextureSwizzleB = 4; Target.TextureSwizzleA = 4; break;
						case ( uint32_t )TextureSetting::ONE:   Target.TextureSwizzleR = 5; Target.TextureSwizzleG = 5; Target.TextureSwizzleB = 5; Target.TextureSwizzleA = 5; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_WRAP_S:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::CLAMP_TO_EDGE:        Target.TextureWrapS = 0; break;
						case ( uint32_t )TextureSetting::CLAMP_TO_BORDER:      Target.TextureWrapS = 1; break;
						case ( uint32_t )TextureSetting::MIRRORED_REPEAT:      Target.TextureWrapS = 2; break;
						case ( uint32_t )TextureSetting::REPEAT:               Target.TextureWrapS = 3; break;
						case ( uint32_t )TextureSetting::MIRROR_CLAMP_TO_EDGE: Target.TextureWrapS = 4; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_WRAP_T:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::CLAMP_TO_EDGE:        Target.TextureWrapT = 0; break;
						case ( uint32_t )TextureSetting::CLAMP_TO_BORDER:      Target.TextureWrapT = 1; break;
						case ( uint32_t )TextureSetting::MIRRORED_REPEAT:      Target.TextureWrapT = 2; break;
						case ( uint32_t )TextureSetting::REPEAT:               Target.TextureWrapT = 3; break;
						case ( uint32_t )TextureSetting::MIRROR_CLAMP_TO_EDGE: Target.TextureWrapT = 4; break;
						default: break;
					}
				break;
			}
			case TextureParameter::TEXTURE_WRAP_R:
			{
				if constexpr ( std::is_integral_v< T > )
					switch ( a_Value )
					{
						case ( uint32_t )TextureSetting::CLAMP_TO_EDGE:        Target.TextureWrapR = 0; break;
						case ( uint32_t )TextureSetting::CLAMP_TO_BORDER:      Target.TextureWrapR = 1; break;
						case ( uint32_t )TextureSetting::MIRRORED_REPEAT:      Target.TextureWrapR = 2; break;
						case ( uint32_t )TextureSetting::REPEAT:               Target.TextureWrapR = 3; break;
						case ( uint32_t )TextureSetting::MIRROR_CLAMP_TO_EDGE: Target.TextureWrapR = 4; break;
						default: break;
					}
				break;
			}
			default:
				break;
		}
	}

	inline static BufferRegistry                  s_BufferRegistry;
	inline static ArrayRegistry                   s_ArrayRegistry;
	inline static TextureRegistry                 s_TextureRegistry;
	inline static AttributeRegistry               s_AttributeRegistry;
	inline static ShaderRegistry                  s_ShaderRegistry;
	inline static ShaderProgramRegistry           s_ShaderProgramRegistry;
	inline static ArrayHandle                     s_ActiveArray;
	inline static ShaderProgramHandle             s_ActiveShaderProgram;
	inline static std::array< BufferHandle, 14 >  s_BufferTargets;
	inline static UniformMap                      s_UniformMap;
	inline static DataStorage< float >            s_VertexStorage;
	inline static DataStorage< Vector4 >          s_PositionStorage;
	inline static DataStorage< float >            s_InterpolatedStorage;
	inline static StrideRegistry                  s_VaryingStrides;
	inline static RenderState                     s_RenderState;
	inline static DepthBuffer                     s_DepthBuffer;
	inline static Pixel                           s_ClearColour;
	inline static float                           s_ClearDepth;
	inline static std::array< TextureUnit, 32 >   s_TextureUnits;
	inline static uint32_t                        s_ActiveTextureUnit;
	inline static uint32_t                        s_ActiveTextureTarget;
};