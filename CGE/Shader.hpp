#pragma once
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Colour.hpp"

//class Uniform
//{
//public:
//
//	inline static void ResizeBuffer( size_t a_Size )
//	{
//		s_Buffer.resize( a_Size );
//	}
//
//	template < typename T >
//	inline static void BindUniform( const T& a_Object, size_t a_Offset )
//	{
//		std::memcpy( s_Buffer.data(), &a_Object + a_Offset, sizeof( T ) );
//	}
//
//	template < typename T >
//	inline static const T& GetUniform( size_t a_Offset )
//	{
//		return *reinterpret_cast< T* >( s_Buffer.data() + a_Offset );
//	}
//
//	inline static size_t GetBufferSize()
//	{
//		return s_Buffer.size();
//	}
//
//private:
//
//	static std::vector< char > s_Buffer;
//};
//
//
//
////class Fragment
////{
////	Vector3 Position;
////	Vector3 Normal;
////	Vector2 Texel;
////};
//
//class Sampler2D
//{
//	typedef void( *EdgeFilter )( Vector2& );
//
//public:
//
//	enum class EdgeMode : char
//	{
//		None,
//		Repeat,
//		MirroredRepeat,
//		ClampToEdge,
//		MirroredClampToEdge
//	};
//
//	static void SetTexture( const Texture& a_Texture )
//	{
//		s_Texture = &a_Texture;
//	}
//
//	inline static const Texture* GetTexture()
//	{
//		return s_Texture;
//	}
//
//	static void SetEdgeMode( EdgeMode a_EdgeMode )
//	{
//		s_EdgeMode = a_EdgeMode;
//
//		switch ( s_EdgeMode )
//		{
//		case Sampler2D::EdgeMode::None:
//		{
//			s_ActiveEdgeFilter = EdgeFilter_None;
//			return;
//		}
//		case Sampler2D::EdgeMode::Repeat:
//		{
//			s_ActiveEdgeFilter = EdgeFilter_Repeat;
//			return;
//		}
//		case Sampler2D::EdgeMode::MirroredRepeat:
//		{
//			s_ActiveEdgeFilter = EdgeFilter_MirroredRepeat;
//			return;
//		}
//		case Sampler2D::EdgeMode::ClampToEdge:
//		{
//			s_ActiveEdgeFilter = EdgeFilter_ClampToEdge;
//			return;
//		}
//		case Sampler2D::EdgeMode::MirroredClampToEdge:
//		{
//			s_ActiveEdgeFilter = EdgeFilter_MirroredClampToEdge;
//			return;
//		}
//		}
//	}
//
//	static EdgeMode GetEdgeMode()
//	{
//		return s_EdgeMode;
//	}
//
//	static Colour Sample( Vector2 a_Texel )
//	{
//		s_ActiveEdgeFilter( a_Texel );
//		//sample
//		return Colour();
//	}
//
//private:
//
//	static void EdgeFilter_None( Vector2& ) { }
//
//	static void EdgeFilter_Repeat( Vector2& a_Texel )
//	{
//
//	}
//
//	static void EdgeFilter_MirroredRepeat( Vector2& a_Texel )
//	{
//
//	}
//
//	static void EdgeFilter_ClampToEdge( Vector2& a_Texel )
//	{
//
//	}
//
//	static void EdgeFilter_MirroredClampToEdge( Vector2& a_Texel )
//	{
//
//	}
//
//	static EdgeFilter     s_ActiveEdgeFilter;
//	static EdgeMode       s_EdgeMode;
//	static const Texture* s_Texture;
//};
//
//class Shader
//{
//	typedef void( *VertexShader )();
//	typedef void( *FragmentShader )();
//
//public:
//
//	enum class ShaderType
//	{
//		Custom,
//		FlatColour,
//		Phong,
//		VertexColour,
//		NormalMap
//	};
//
//	bool SetShaderType( ShaderType a_ShaderType )
//	{
//		switch ( m_ShaderType )
//		{
//		case Shader::ShaderType::Custom:
//		{
//			return false;
//		}
//		case Shader::ShaderType::FlatColour:
//		{
//			m_VertexShader = VertexShader_FlatColour;
//			m_FragmentShader = FragmentShader_FlatColour;
//			break;
//		}
//		case Shader::ShaderType::Phong:
//		{
//			m_VertexShader = VertexShader_Phong;
//			m_FragmentShader = FragmentShader_Phong;
//			break;
//		}
//		case Shader::ShaderType::VertexColour:
//		{
//			m_VertexShader = VertexShader_VertexColour;
//			m_FragmentShader = FragmentShader_VertexColour;
//			break;
//		}
//		case Shader::ShaderType::NormalMap:
//		{
//			m_VertexShader = VertexShader_NormalMap;
//			m_FragmentShader = FragmentShader_NormalMap;
//			break;
//		}
//		}
//
//		m_ShaderType = a_ShaderType;
//		return true;
//	}
//
//	inline ShaderType GetShadeType() const
//	{
//		return m_ShaderType;
//	}
//
//	inline bool SetShaderFunctions( VertexShader a_VertexShader, FragmentShader a_FragmentShader )
//	{
//		if ( a_VertexShader && a_FragmentShader )
//		{
//			m_ShaderType = ShaderType::Custom;
//			m_VertexShader = a_VertexShader;
//			m_FragmentShader = a_FragmentShader;
//			return true;
//		}
//
//		return false;
//	}
//
//private:
//
//	static void VertexShader_FlatColour()
//	{
//
//	}
//
//	static void VertexShader_Phong()
//	{
//
//	}
//
//	static void VertexShader_VertexColour()
//	{
//
//	}
//
//	static void VertexShader_NormalMap()
//	{
//
//	}
//
//	static void FragmentShader_FlatColour()
//	{
//
//	}
//
//	static void FragmentShader_Phong()
//	{
//
//	}
//
//	static void FragmentShader_VertexColour()
//	{
//
//	}
//
//	static void FragmentShader_NormalMap()
//	{
//
//	}
//
//	VertexShader   m_VertexShader;
//	FragmentShader m_FragmentShader;
//	ShaderType     m_ShaderType;
//};