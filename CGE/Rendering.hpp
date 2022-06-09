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

#define Uniform( Type, Name ) auto& ##Name = Rendering::Uniform< crc32_cpt( __FUNCTION__ ), Type, #Name##_H >::Value();
#define Attribute( Location, Type, Name ) auto& ##Name = Rendering::Attribute< Location, Type >::Value();
#define Varying_In( Type, Name ) auto& ##Name = Rendering::Varying< crc32_cpt( __FUNCTION__ ), Type, #Name##""_H >::In();
#define Varying_Out( Type, Name ) auto& ##Name = Rendering::Varying< crc32_cpt( __FUNCTION__ ), Type, #Name##""_H >::Out();

template < Hash _ShaderName >
void* ShaderAddress = nullptr;

typedef uint32_t BufferHandle;
typedef uint32_t ArrayHandle;
typedef uint32_t TextureHandle;
typedef uint32_t ShaderHandle;
typedef uint32_t ShaderProgramHandle;

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

// Analogous to OpenGL
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
	static void GenTextures( size_t a_Count, TextureHandle* a_Handles );
	static void BindTexture( TextureTarget a_TextureTarget, TextureHandle a_Handle );
	static void TexImage1D( /*something*/ );
	static void TexImage2D( TextureTarget a_TextureTarget, uint8_t a_MipMapLevel, TextureFormat a_InternalFormat, int32_t a_Width, int32_t a_Height, int32_t a_Border, TextureFormat a_TextureFormat, DataType a_DataType, void* a_Data );
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
	//static void Uniform

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
			return *reinterpret_cast< _Type* >( reinterpret_cast< uint8_t* >( s_VertexDataStorage.Interpolated().Data() ) + s_Offset );
		};

		static _Type& Out()
		{
			static OnStart Setup = s_Setup;
			return *reinterpret_cast< _Type* >( s_VertexDataStorage.Head() + s_Offset );
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

		inline void Rewind()
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
	class TextureBuffer
	{
	public:

		TextureBuffer()
			: TextureMinLOD( -1000 )
			, TextureMaxLOD( 1000 )
			, TextureMaxLevel( 1000 )
			, TextureSwizzleR( 0 )
			, TextureSwizzleG( 1 )
			, TextureSwizzleB( 2 )
			, TextureSwizzleA( 3 )
		{ }

		uint8_t DepthStencilTextureMode : 1;
		int32_t TextureBaseLevel;
		Vector4 TextureBorderColour;
		uint8_t TextureCompareFunc      : 3;
		uint8_t TextureCompareMode      : 1;
		float   TextureLODBias;
		int8_t  TextureMagFilter        : 1;
		int8_t  TextureMinFilter        : 3;
		float   TextureMinLOD;
		float   TextureMaxLOD;
		int32_t TextureMaxLevel;
		int8_t  TextureSwizzleR         : 3;
		int8_t  TextureSwizzleG         : 3;
		int8_t  TextureSwizzleB         : 3;
		int8_t  TextureSwizzleA         : 3;
		uint8_t TextureWrapS            : 3;
		uint8_t TextureWrapT            : 3;
		uint8_t TextureWrapR            : 3;

	};

	typedef std::vector< uint8_t >           Buffer;
	typedef std::array< VertexAttribute, 8 > Array;
	typedef std::map< void*, uint32_t >      StrideRegistry;

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
			return Index;
		}

		void Destroy( BufferHandle a_Handle )
		{
			m_Availability[ a_Handle - 1 ] = false;
			//m_Buffers[ a_Handle - 1 ];
		}

		inline TextureBuffer& operator[]( BufferHandle a_Handle )
		{
			return m_Buffers[ a_Handle - 1 ];
		}

		inline bool Valid( TextureHandle a_Handle )
		{
			return a_Handle - 1 < m_Availability.size() && m_Availability[ a_Handle - 1 ];
		}

	private:

		std::bitset< 32 >               m_Availability;
		std::array< TextureBuffer, 32 > m_Buffers;
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

		inline void Rewind( uint32_t a_Position )
		{
			m_VertexAttributes[ a_Position ].Rewind();
		}

		inline void ResetAll()
		{
			m_VertexAttributes[ 0 ].Rewind();
			m_VertexAttributes[ 1 ].Rewind();
			m_VertexAttributes[ 2 ].Rewind();
			m_VertexAttributes[ 3 ].Rewind();
			m_VertexAttributes[ 4 ].Rewind();
			m_VertexAttributes[ 5 ].Rewind();
			m_VertexAttributes[ 6 ].Rewind();
			m_VertexAttributes[ 7 ].Rewind();
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
	class VertexDataStorage
	{
	public:

		VertexDataStorage()
			: m_Stride( 0 )
			, m_Head( nullptr )
		{ }

		void Prepare( uint32_t a_Size, uint32_t a_Stride )
		{
			if ( a_Size * a_Stride > m_Data.size() )
			{
				m_Data.resize( a_Size * a_Stride );
			}

			if ( a_Stride / sizeof( float ) > m_Interpolated.Size() )
			{
				m_Interpolated.Resize( a_Stride / sizeof( float ) );
			}

			m_Stride = a_Stride;
			m_Head   = m_Data.data();
		}

		void Increment()
		{
			m_Head += m_Stride;
		}

		void Rewind()
		{
			m_Head = m_Data.data();
		}

		uint8_t* Data()
		{
			return m_Data.data();
		}

		uint8_t* Head()
		{
			return m_Head;
		}

		AttribVector& Interpolated()
		{
			return m_Interpolated;
		}

	private:

		uint32_t               m_Stride;
		uint8_t*               m_Head;
		std::vector< uint8_t > m_Data;
		AttribVector           m_Interpolated;
	};
	class PositionDataStorage
	{
	public:

		PositionDataStorage()
			: m_Head( nullptr )
		{ }

		void Prepare( uint32_t a_Size )
		{
			if ( a_Size  > m_Positions.size() )
			{
				m_Positions.resize( a_Size );
			}

			m_Head = m_Positions.data();
		}

		void Increment()
		{
			++m_Head;
		}

		void Rewind()
		{
			m_Head = m_Positions.data();
		}

		inline Vector4* Head()
		{
			return m_Head;
		}

		void operator=( const Vector4& a_Value )
		{
			*m_Head = a_Value;
		}

	private:

		Vector4*               m_Head;
		std::vector< Vector4 > m_Positions;
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

	template < uint32_t _Size >
	static void DrawArraysImpl( uint32_t a_Begin, uint32_t a_Count )
	{

	}

	template <>
	static void DrawArraysImpl< 3 >( uint32_t a_Begin, uint32_t a_Count )
	{
		float minDepth = 1000.0f;
		float maxDepth = -1000.0f;


		auto& ActiveProgram = s_ShaderProgramRegistry[ s_ActiveShaderProgram ];
		auto& ActiveArray   = s_ArrayRegistry[ s_ActiveArray ];
		
		// Setup vertex data storage.
		void( *VertexShader )( ) = ActiveProgram[ ShaderType::VERTEX_SHADER ];
		void( *FragmentShader )( ) = ActiveProgram[ ShaderType::FRAGMENT_SHADER ];
		uint32_t VertexStride = s_VaryingStrides[ VertexShader ];
		uint32_t AttribStride = VertexStride / sizeof( float );
		s_VertexDataStorage.Prepare( a_Count, VertexStride );

		// Setup position data storage.
		s_PositionDataStorage.Prepare( a_Count );

		for ( uint32_t Begin = a_Begin, End = a_Begin + a_Count; Begin < End; ++Begin, s_AttributeRegistry.IncrementAll(), s_VertexDataStorage.Increment(), s_PositionDataStorage.Increment() )
		{
			VertexShader();

			float w = Position.w;
			Position /= Position.w;
			Position.w = 1.0f / w;

			float* VertexData = ( float* )s_VertexDataStorage.Head();

			for ( uint32_t i = 0; i < AttribStride; ++i )
			{
				VertexData[ i ] /= w;
			}
			
			Position.x *= 0.5f * ConsoleWindow::GetCurrentContext()->GetWidth();
			Position.y *= 0.5f * ConsoleWindow::GetCurrentContext()->GetHeight();
			Position.x += 0.5f * ConsoleWindow::GetCurrentContext()->GetWidth();
			Position.y += 0.5f * ConsoleWindow::GetCurrentContext()->GetHeight();
			s_PositionDataStorage = Position;
		}
		
		// Reset position and vertex storage.
		s_PositionDataStorage.Rewind();
		s_VertexDataStorage.Rewind();

		Vector4* Positions[ 3 ];
		float*   Vertices[ 3 ];

		for ( uint32_t Begin = a_Begin, End = a_Begin + a_Count; Begin < End; )
		{
			for ( uint32_t i = 0; i < 3; ++i, ++Begin, s_PositionDataStorage.Increment(), s_VertexDataStorage.Increment() )
			{
				Positions[ i ] = s_PositionDataStorage.Head();
				Vertices[ i ] = reinterpret_cast< float* >( s_VertexDataStorage.Head() );
			}

			Vector4 
				*a_P0 = Positions[ 0 ],
				*a_P1 = Positions[ 1 ],
				*a_P2 = Positions[ 2 ];

			// Move this out into Rendering settings for backface culling

			bool FacingForward = Math::Cross( Vector3( *a_P1 - *a_P0 ), Vector3( *a_P2 - *a_P0 ) ).z > 0.0f;

			if ( s_RenderState.CullFace )
			{
				if ( s_RenderState.FrontCull && FacingForward )
				{
					continue;
				}

				if ( s_RenderState.BackCull && !FacingForward )
				{
					continue;
				}
			}

			AttribVector
				a_C0( AttribStride, Vertices[ 0 ] ),
				a_C1( AttribStride, Vertices[ 1 ] ),
				a_C2( AttribStride, Vertices[ 2 ] );
			
			// Correct Y
			a_P0->y = -a_P0->y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();
			a_P1->y = -a_P1->y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();
			a_P2->y = -a_P2->y - 1.0f + ConsoleWindow::GetCurrentContext()->GetHeight();

			// Sort corners.
			if ( a_P0->y < a_P1->y )
			{
				std::swap( a_P0, a_P1 );
				std::swap( a_C0, a_C1 );
			}

			if ( a_P0->y < a_P2->y )
			{
				std::swap( a_P0, a_P2 );
				std::swap( a_C0, a_C2 );
			}

			if ( a_P1->y < a_P2->y )
			{
				std::swap( a_P1, a_P2 );
				std::swap( a_C1, a_C2 );
			}

			// Align y.
			a_P0->y = static_cast< int >( a_P0->y );
			a_P1->y = static_cast< int >( a_P1->y );
			a_P2->y = static_cast< int >( a_P2->y );

			if ( a_P0->y == a_P2->y )
			{
				return;
			}

			// Find M
			float L = ( a_P1->y - a_P2->y ) / ( a_P0->y - a_P2->y );
			Vector4 PM = Math::Lerp( L, *a_P2, *a_P0 );
			PM.y = static_cast< int >( PM.y );

			AttribVector CM( AttribStride );

			for ( uint32_t i = 0; i < AttribStride; ++i )
			{
				CM[ i ] = Math::Lerp( L, a_C2[ i ], a_C0[ i ] );
			}

			// Swap P1 and M if M is not on right.
			if ( PM.x < a_P1->x )
			{
				std::swap( PM, *a_P1 );
				std::swap( CM, a_C1 );
			}

			float SpanX, SpanY, y;
			Vector4 PStepL, PStepR, PBegin, PStep, PL, PR;
			//Vector4 CStepL, CStepR, CBegin, CStep, CL, CR;
			AttribVector 
				CStepL( AttribStride ), 
				CStepR( AttribStride ), 
				CBegin( AttribStride ), 
				CStep( AttribStride ), 
				CL( AttribStride ), 
				CR( AttribStride );

			if ( a_P2->y != a_P1->y )
			{
				SpanY = 1.0f / ( a_P1->y - a_P2->y );

				PStepL = *a_P1 - *a_P2; 
				PStepL *= SpanY;
				PStepR = PM - *a_P2; 
				PStepR *= SpanY;
				PL = *a_P2;
				PR = *a_P2;

				CStepL = a_C1;
				CStepL -= a_C2; 
				CStepL *= SpanY;
				CStepR = CM;
				CStepR -= a_C2; 
				CStepR *= SpanY;
				CL = a_C2;
				CR = a_C2;

				y = a_P2->y;

				for ( ; y < a_P1->y; ++y )
				{
					PL += PStepL;
					PR += PStepR;
					CL += CStepL;
					CR += CStepR;

					SpanX = 1.0f / ( PR.x - PL.x );
					PBegin = PL;
					PStep = ( PR - PL ) * SpanX;
					CBegin = CL;
					CStep = CR;
					CStep -= CL; 
					CStep *= SpanX;

					for ( ; PBegin.x < static_cast< int >( PR.x ); PBegin += PStep, CBegin += CStep )
					{
						if ( PBegin.z / PBegin.w < minDepth )
						{
							minDepth = PBegin.z;
						}

						if ( PBegin.z / PBegin.w > maxDepth )
						{
							maxDepth = PBegin.z;
						}

						if ( s_RenderState.DepthTest && !s_DepthBuffer.TestAndCommit( PBegin.x, PBegin.y, PBegin.z / PBegin.w ) )
						{
							continue;
						}

						s_VertexDataStorage.Interpolated() = CBegin;
						s_VertexDataStorage.Interpolated() /= PBegin.w;
						_FragCoord = PBegin;
						FragmentShader();
						Colour Fragment( 255 * FragColour[ 0 ], 255 * FragColour[ 1 ], 255 * FragColour[ 2 ], 255 * FragColour[ 3 ] );
						ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetColour( { PBegin.x, y }, Fragment );
					}
				}
			}
			else
			{
				y = a_P1->y;
				PL = *a_P1;
				PR = PM;
				CL = a_C1;
				CR = CM;
			}

			if ( a_P1->y != a_P0->y )
			{
				SpanY = 1.0f / ( a_P0->y - a_P1->y );

				PStepL = *a_P0 - *a_P1; PStepL *= SpanY;
				PStepR = *a_P0 - PM; PStepR *= SpanY;

				CStepL = a_C0;
				CStepL -= a_C1; 
				CStepL *= SpanY;
				CStepR = a_C0;
				CStepR -= CM; 
				CStepR *= SpanY;

				for ( ; y < a_P0->y; ++y )
				{
					PL += PStepL;
					PR += PStepR;
					CL += CStepL;
					CR += CStepR;

					SpanX = 1.0f / ( PR.x - PL.x );
					PBegin = PL;
					PStep = ( PR - PL ) * SpanX;
					CBegin = CL;
					CStep = CR;
					CStep -= CL;
					CStep *= SpanX;

					for ( ; PBegin.x < static_cast< int >( PR.x ); PBegin += PStep, CBegin += CStep )
					{
						if ( PBegin.z / PBegin.w < minDepth )
						{
							minDepth = PBegin.z;
						}

						if ( PBegin.z / PBegin.w > maxDepth )
						{
							maxDepth = PBegin.z;
						}

						if ( s_RenderState.DepthTest && !s_DepthBuffer.TestAndCommit( PBegin.x, PBegin.y, PBegin.z / PBegin.w ) )
						{
							continue;
						}

						s_VertexDataStorage.Interpolated() = CBegin;
						s_VertexDataStorage.Interpolated() /= PBegin.w;
						_FragCoord = PBegin;
						FragmentShader();
						Colour Fragment( 255 * FragColour[ 0 ], 255 * FragColour[ 1 ], 255 * FragColour[ 2 ], 255 * FragColour[ 3 ] );
						ConsoleWindow::GetCurrentContext()->GetScreenBuffer().SetColour( { PBegin.x, y }, Fragment );
					}
				}
			}
		}

		float _min = minDepth;
		float _max = maxDepth;
	}

	template < typename T >
	static void TexParameterImpl( TextureTarget a_TextureTarget, TextureParameter a_TextureParameter, T a_Value )
	{
		TextureBuffer& Target = s_TextureRegistry[ s_TextureTargets[ ( uint32_t )a_TextureTarget ] ];

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
		TextureBuffer& Target = s_TextureRegistry[ a_Handle ];

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


	inline static BufferRegistry        s_BufferRegistry;
	inline static ArrayRegistry         s_ArrayRegistry;
	inline static TextureRegistry       s_TextureRegistry;
	inline static AttributeRegistry     s_AttributeRegistry;
	inline static ShaderRegistry        s_ShaderRegistry;
	inline static ShaderProgramRegistry s_ShaderProgramRegistry;
	inline static ArrayHandle           s_ActiveArray;
	inline static ShaderProgramHandle   s_ActiveShaderProgram;
	inline static BufferHandle          s_BufferTargets[ 14 ];
	inline static TextureHandle         s_TextureTargets[ 17 ];
	inline static UniformMap            s_UniformMap;
	inline static VertexDataStorage     s_VertexDataStorage;
	inline static PositionDataStorage   s_PositionDataStorage;
	inline static StrideRegistry        s_VaryingStrides;
	inline static RenderState           s_RenderState;
	inline static DepthBuffer           s_DepthBuffer;
	inline static Pixel                 s_ClearColour;
	inline static float                 s_ClearDepth;

};