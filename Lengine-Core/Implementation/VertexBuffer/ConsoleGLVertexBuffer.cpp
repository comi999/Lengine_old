#include "ConsoleGLShader.hpp"
#include "ConsoleGL.hpp"

bool ConsoleGLShader::SetInt( const char* a_Name, const int32_t* a_Values, size_t a_Count )
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	auto Location = ConsoleGL::GetUniformLocation( m_ShaderProgramHandle, a_Name );

	if ( Location == -1 )
	{
		return false;
	}

	ConsoleGL::Uniform1iv( Location, a_Count, a_Values );
}

bool ConsoleGLShader::SetUInt( const char* a_Name, const uint32_t* a_Values, size_t a_Count )
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	auto Location = ConsoleGL::GetUniformLocation( m_ShaderProgramHandle, a_Name );

	if ( Location == -1 )
	{
		return false;
	}

	ConsoleGL::Uniform1uiv( Location, a_Count, a_Values );
}

bool ConsoleGLShader::SetFloat( const char* a_Name, const float* a_Values, size_t a_Count )
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	auto Location = ConsoleGL::GetUniformLocation( m_ShaderProgramHandle, a_Name );

	if ( Location == -1 )
	{
		return false;
	}

	ConsoleGL::Uniform1fv( Location, a_Count, a_Values );
}

bool ConsoleGLShader::SetMatrix4( const char* a_Name, const Matrix4* a_Values, size_t a_Count )
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	auto Location = ConsoleGL::GetUniformLocation( m_ShaderProgramHandle, a_Name );

	if ( Location == -1 )
	{
		return false;
	}

	ConsoleGL::UniformMatrix4fv( Location, a_Count, false, a_Values->Data );
}