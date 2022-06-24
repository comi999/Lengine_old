#pragma once
#include "Rendering.hpp"
#include "Resource.hpp"
#include "Hash.hpp"

DefineShader( Default_Vertex )
{
	Uniform( Matrix4, u_PVM );
	Attribute( 0, Vector4, a_Position );
	Rendering::Position = Math::Multiply( u_PVM, a_Position );
}

DefineShader( Default_Fragment )
{
	Rendering::FragColour = Vector4::One;
}

class Shader : public Resource
{
public:

	Shader()
		: m_VertexShaderSource( "Shader_Default_Vertex" )
		, m_FragmentShaderSource( "Shader_Default_Fragment" )
	{ }

	Shader( const std::string& a_VertexShaderSource, const std::string& a_FragmentShaderSource )
		: m_VertexShaderSource( a_VertexShaderSource )
		, m_FragmentShaderSource( a_FragmentShaderSource )
	{ }

	void SetSource( ShaderType a_ShaderType, const char* a_Source )
	{
		std::string* Source = nullptr;

		switch ( a_ShaderType )
		{
			case ShaderType::VERTEX_SHADER:   Source = &m_VertexShaderSource;   break;
			case ShaderType::FRAGMENT_SHADER: Source = &m_FragmentShaderSource; break;
		}

		if ( !Source )
		{
			return;
		}

		*Source = a_Source;

		if ( !m_ShaderProgramHandle )
		{
			return;
		}

		Decompile();
		Compile();
	}

	const char* GetSource( ShaderType a_ShaderType )
	{
		switch ( a_ShaderType )
		{
			case ShaderType::VERTEX_SHADER: return m_VertexShaderSource.c_str();
			case ShaderType::FRAGMENT_SHADER: return m_FragmentShaderSource.c_str();
		}

		return nullptr;
	}

	void SetColour( const char* a_Name, Colour a_Colour )
	{
		uint32_t Location = Rendering::GetUniformLocation( m_ShaderProgramHandle, a_Name );
		
		if ( Location != -1 )
		{
			Vector4 Colour = a_Colour.Normalized();
			Rendering::Uniform4f( Location, Colour.x, Colour.y, Colour.z, Colour.w );
		}
	}

	// void SetInt/Float/Bool/Vector/Matrix

	void Compile()
	{
		if ( m_ShaderProgramHandle != 0 )
		{
			return;
		}

		ShaderHandle VertexShaderID = Rendering::CreateShader( ShaderType::VERTEX_SHADER );
		ShaderHandle FragmentShaderID = Rendering::CreateShader( ShaderType::FRAGMENT_SHADER );

		auto VertexShaderIter = Internal::ShaderFuncLookup.find( CRC32_RT( m_VertexShaderSource.c_str() ) );
		auto FragmentShaderIter = Internal::ShaderFuncLookup.find( CRC32_RT( m_FragmentShaderSource.c_str() ) );

		const void* VertexSource = VertexShaderIter != Internal::ShaderFuncLookup.end() ? VertexShaderIter->second : nullptr;
		const void* FragmentSource = FragmentShaderIter != Internal::ShaderFuncLookup.end() ? FragmentShaderIter->second : nullptr;

		Rendering::ShaderSource( VertexShaderID, 1, &VertexSource, nullptr );
		Rendering::CompileShader( VertexShaderID );
		Rendering::ShaderSource( FragmentShaderID, 1, &FragmentSource, nullptr );
		Rendering::CompileShader( FragmentShaderID );

		m_ShaderProgramHandle = Rendering::CreateProgram();
		Rendering::AttachShader( m_ShaderProgramHandle, VertexShaderID );
		Rendering::AttachShader( m_ShaderProgramHandle, FragmentShaderID );
		Rendering::LinkProgram( m_ShaderProgramHandle );

		Rendering::DetachShader( m_ShaderProgramHandle, VertexShaderID );
		Rendering::DetachShader( m_ShaderProgramHandle, FragmentShaderID );
		Rendering::DeleteShader( VertexShaderID );
		Rendering::DeleteShader( FragmentShaderID );
	}

	void Decompile()
	{
		if ( m_ShaderProgramHandle == 0 )
		{
			return;
		}

		Rendering::DeleteProgram( m_ShaderProgramHandle );
	}

	ShaderProgramHandle GetProgramHandle() const
	{
		return m_ShaderProgramHandle;
	}

private:

	friend class ResourcePackager;
	friend class Serialization;

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << m_VertexShaderSource;
		a_Serializer << m_FragmentShaderSource;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> m_VertexShaderSource;
		a_Deserializer >> m_FragmentShaderSource;
	}

	ShaderProgramHandle m_ShaderProgramHandle;
	std::string         m_VertexShaderSource;
	std::string         m_FragmentShaderSource;

public:

	static Shader Default;
};