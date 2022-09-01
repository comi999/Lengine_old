#pragma once
#include <stdint.h>
#include <array>
#include <vector>
#include <bitset>
#include <type_traits>
#include <map>
#include "Math.hpp"
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
	COLOUR_BUFFER_BIT = 1u << 0,
	DEPTH_BUFFER_BIT = 1u << 1,
	ACCUM_BUFFER_BIT = 1u << 2,
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
	DRAW,
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

enum class RenderMode : uint8_t
{
	POINT,
	LINE,
	TRIANGLE
};

enum class ClipPlane : uint8_t
{
	CLIP_PLANE0,
	CLIP_PLANE1,
	CLIP_PLANE2,
	CLIP_PLANE3,
	CLIP_PLANE4,
	CLIP_PLANE5,
	CLIP_PLANE6,
	CLIP_PLANE7,
	CLIP_PLANE8,
	CLIP_PLANE9,
	CLIP_PLANE10,
	CLIP_PLANE11,
	CLIP_PLANE12,
	CLIP_PLANE13,
	CLIP_PLANE14,
	CLIP_PLANE15
};

#define DefineShader( Name ) \
void Shader_##Name ();       \
template <> void* Internal::ShaderAddress< "Shader_"#Name##_H > = Shader_##Name; \
namespace Internal { bool _ShaderRegistered_##Name = RegisterShader< "Shader_"#Name##_H >::Registered; }; \
void Shader_##Name ()

#define Uniform( Type, Name ) auto& ##Name = Rendering::Uniform< crc32_cpt( __FUNCTION__ ), Type, #Name##_H >::Value()
#define Attribute( Location, Type, Name ) auto& ##Name = Rendering::Property< Location, Type >::Value()
#define Varying_In( Type, Name ) auto& ##Name = Rendering::Varying< crc32_cpt( __FUNCTION__ ), Type, #Name##""_H >::In()
#define Varying_Out( Type, Name ) auto& ##Name = Rendering::Varying< crc32_cpt( __FUNCTION__ ), Type, #Name##""_H >::Out()
#define InOut( Type, Name ) auto& ##Name = Rendering::InOut< Type, #Name##""_H >::Value()

namespace Internal
{
	template < Hash _ShaderName >
	void* ShaderAddress = nullptr;

	struct ShaderFuncLookup
	{
		inline static std::map< Hash, void* > Value;
	};

	template < Hash _ShaderName >
	struct RegisterShader
	{
		inline static bool Registered = []()
		{
			ShaderFuncLookup::Value.emplace( _ShaderName, ShaderAddress< _ShaderName > );
			return true;
		}( );
	};

} // namespace Internal

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

struct ShaderObject
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

	static void Empty() {}

public:

	ShaderType Type = ShaderType::INVALID;
	void( *Callback )( ) = nullptr;
};

class ShaderProgram
{
private:

	friend class Rendering;
	friend class ShaderProgramRegistry;

	typedef void( *ShaderCallback )( );

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

		inline ShaderObject& operator[]( ShaderHandle a_Handle )
		{
			return m_Shaders[ a_Handle - 1 ];
		}

		inline bool Valid( ShaderHandle a_Handle )
		{
			return a_Handle - 1 < m_Availability.size() && m_Availability[ a_Handle - 1 ];
		}

	private:

		std::bitset< 32 >        m_Availability;
		std::array< ShaderObject, 32 > m_Shaders;
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

class ConsoleRendering
{
public:

	// Vertex out variables.
	inline static Vector4 Position;

	// Fragment out variables.
	inline static Vector4 FragColour;

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
	static void DrawElements( RenderMode a_Mode, uint32_t a_Count, DataType a_DataType, const void* a_Indices );
	static void Enable( RenderSetting a_RenderSetting );
	static void Disable( RenderSetting a_RenderSetting );
	static void CullFace( CullFaceMode a_CullFace );
	static void DepthFunc( TextureSetting a_TextureSetting );

	static void GetBooleanv( RenderSetting a_RenderSetting, bool* a_Value );
	// Need the other Get functions.

	static void ClipPlane( const double* a_Equation );

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

	//static void TexImage1D( /*something*/ );
	static void TexImage2D( TextureTarget a_TextureTarget, uint8_t a_MipMapLevel, TextureFormat a_InternalFormat, int32_t a_Width, int32_t a_Height, int32_t a_Border, TextureFormat a_TextureFormat, TextureSetting a_DataLayout, const void* a_Data );
	//static void TexImage3D( /*something*/ );
	//static void GenerateMipmap( TextureTarget a_TextureTarget );

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
	static void Uniform1fv( int32_t a_Location, uint32_t a_Count, const float* a_Value );
	static void Uniform2fv( int32_t a_Location, uint32_t a_Count, const float* a_Value );
	static void Uniform3fv( int32_t a_Location, uint32_t a_Count, const float* a_Value );
	static void Uniform4fv( int32_t a_Location, uint32_t a_Count, const float* a_Value );
	static void Uniform1iv( int32_t a_Location, uint32_t a_Count, const int32_t* a_Value );
	static void Uniform2iv( int32_t a_Location, uint32_t a_Count, const int32_t* a_Value );
	static void Uniform3iv( int32_t a_Location, uint32_t a_Count, const int32_t* a_Value );
	static void Uniform4iv( int32_t a_Location, uint32_t a_Count, const int32_t* a_Value );
	static void Uniform1uiv( int32_t a_Location, uint32_t a_Count, const uint32_t* a_Value );
	static void Uniform2uiv( int32_t a_Location, uint32_t a_Count, const uint32_t* a_Value );
	static void Uniform3uiv( int32_t a_Location, uint32_t a_Count, const uint32_t* a_Value );
	static void Uniform4uiv( int32_t a_Location, uint32_t a_Count, const uint32_t* a_Value );
	static void UniformMatrix2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );
	static void UniformMatrix3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );
	static void UniformMatrix4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );
	static void UniformMatrix2x3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );
	static void UniformMatrix3x2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );
	static void UniformMatrix2x4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );
	static void UniformMatrix4x2fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );
	static void UniformMatrix3x4fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );
	static void UniformMatrix4x3fv( uint32_t a_Location, uint32_t a_Count, bool a_Transpose, const float* a_Value );

	
};