#pragma once
#include "Rendering.hpp"
#include "Resource.hpp"
#include "Hash.hpp"

class Shader : public Resource
{
public:

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
		VertexShader >> a_Serializer;
		FragmentShader >> a_Serializer;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		m_VertexShaderSource << a_Deserializer;
		m_FragmentShaderSource << a_Deserializer;
	}

	ShaderProgramHandle m_ShaderProgramHandle;
	std::string         m_VertexShaderSource;
	std::string         m_FragmentShaderSource;
};