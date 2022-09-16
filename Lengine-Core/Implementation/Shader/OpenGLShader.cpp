#include "OpenGLShader.hpp"

#define MAX_UNIFORM_NAME 128

bool OpenGLShader::IsCompiled() const
{
	return m_ShaderProgramHandle;
}

bool OpenGLShader::OnCompile()
{
	if ( m_ShaderProgramHandle )
	{
		return true;
	}

	// Compilation code

	GLint UniformCount = 0;
	GLchar UniformName[ MAX_UNIFORM_NAME ];
	GLsizei UniformNameLength = 0;
	GLint UniformSize = 0;
	GLenum UniformType;
	glGetProgramiv( m_ShaderProgramHandle, GL_ACTIVE_UNIFORMS, &UniformCount );

	for ( GLuint i = 0; i < UniformCount; ++i )
	{
		glGetActiveUniform( 
			m_ShaderProgramHandle, 
			i, 
			MAX_UNIFORM_NAME, 
			&UniformNameLength, 
			&UniformSize, 
			&UniformType, 
			UniformName );

		auto UniformLocation = glGetUniformLocation( m_ShaderProgramHandle, UniformName );

		if ( UniformLocation == -1 )
		{
			continue;
		}

		AddShaderInput( Name( UniformName, UniformNameLength ) );
		m_UniformLocations.push_back( UniformLocation );
	}

	return true;
}

bool OpenGLShader::OnDecompile()
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	glDeleteProgram( m_ShaderProgramHandle );
	m_UniformLocations.clear();
	return true;
}

bool OpenGLShader::Use() const
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	glUseProgram( m_ShaderProgramHandle );
	return true;
}

bool OpenGLShader::SetValue( int32_t a_Index, const int32_t* a_Values, size_t a_Count ) const
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	glUniform1iv( m_UniformLocations[ a_Index ], a_Count, a_Values );
}

bool OpenGLShader::SetValue( int32_t a_Index, const uint32_t* a_Values, size_t a_Count ) const
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	glUniform1uiv( m_UniformLocations[ a_Index ], a_Count, a_Values );
}

bool OpenGLShader::SetValue( int32_t a_Index, const float* a_Values, size_t a_Count ) const
{
	if ( !m_ShaderProgramHandle )
	{
		return false;
	}

	glUniform1fv( m_UniformLocations[ a_Index ], a_Count, a_Values );
}