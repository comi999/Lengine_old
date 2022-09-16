#include "ConsoleGLShader.hpp"
#include "../../ConsoleGL.hpp"

bool ConsoleGLShader::IsCompiled() const
{
	return !m_ShaderProgramHandle;
}

bool ConsoleGLShader::Compile() const
{
	
}

bool ConsoleGLShader::Decompile() const
{

}

int32_t ConsoleGLShader::GetIndex( const char* a_Name ) const
{
	if ( !IsCompiled() )
	{
		return -1;
	}

	auto Iter = std::search( m_UniformInfos.begin(), m_UniformInfos.end(), [&]( const UniformInfo& a_UniformInfo ) { return a_UniformInfo.Name == a_Name; } );
	return Iter == m_UniformInfos.end() ? -1 : Iter - m_UniformInfos.begin();
}

bool ConsoleGLShader::SetInt( int32_t a_Index, const int32_t* a_Values, size_t a_Count ) const
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

bool ConsoleGLShader::SetUInt( int32_t a_Index, const uint32_t* a_Values, size_t a_Count ) const
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

bool ConsoleGLShader::SetFloat( int32_t a_Index, const float* a_Values, size_t a_Count ) const
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

bool ConsoleGLShader::SetMatrix4( int32_t a_Index, const Matrix4* a_Values, size_t a_Count ) const
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