#pragma once
#include "Math.hpp"
#include "Rendering.hpp"
#include "Resource.hpp"
#include "Hash.hpp"

class Shader : public Resource
{
public:

	Shader()
		: Shader( "Default"_N, "Vertex_Default", "Fragment_Default" )
	{ }

	Shader( const Name& a_Name, const std::string& a_VertexShaderSource, const std::string& a_FragmentShaderSource )
		: Resource( a_Name )
		, m_ShaderProgramHandle( 0 )
		, m_VertexShaderSource( "Shader_" + a_VertexShaderSource )
		, m_FragmentShaderSource( "Shader_" + a_FragmentShaderSource )
	{ }

	void SetSource( ShaderType a_ShaderType, const char* a_Source )
	{
		std::string* ResourceSource = nullptr;

		switch ( a_ShaderType )
		{
			case ShaderType::VERTEX_SHADER:   ResourceSource = &m_VertexShaderSource;   break;
			case ShaderType::FRAGMENT_SHADER: ResourceSource = &m_FragmentShaderSource; break;
		}

		if ( !ResourceSource )
		{
			return;
		}

		*ResourceSource = a_Source;

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

	//Ideally, shader object should be in charge of setting uniforms and setting up attributes.

	void Compile()
	{
		if ( m_ShaderProgramHandle != 0 )
		{
			return;
		}

		ShaderHandle VertexShaderID = Rendering::CreateShader( ShaderType::VERTEX_SHADER );
		ShaderHandle FragmentShaderID = Rendering::CreateShader( ShaderType::FRAGMENT_SHADER );

		auto VertexShaderIter = Internal::ShaderFuncLookup::Value.find( CRC32_RT( m_VertexShaderSource.c_str() ) );
		auto FragmentShaderIter = Internal::ShaderFuncLookup::Value.find( CRC32_RT( m_FragmentShaderSource.c_str() ) );

		const void* VertexSource = VertexShaderIter != Internal::ShaderFuncLookup::Value.end() ? VertexShaderIter->second : nullptr;
		const void* FragmentSource = FragmentShaderIter != Internal::ShaderFuncLookup::Value.end() ? FragmentShaderIter->second : nullptr;

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
	static Shader Diffuse;
	static Shader Specular;
	static Shader Normal;
	static Shader Phong;
	static Shader UnlitFlatColour;
	static Shader LitFlatColour;
	// More lighting models.
};