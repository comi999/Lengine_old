#pragma once

#include <GL/glew.h>

#include "../../Shader.hpp"

class OpenGLShader : public Shader
{
public:

	RenderingAPI GetAPI() const override { return RenderingAPI::OpenGL; }
	bool IsCompiled() const override;
	bool OnCompile() override;
	bool OnDecompile() override;
	bool Use() const override;
	bool SetValue( int32_t a_Index, const int32_t*  a_Values, size_t a_Count = 1 ) const override;
	bool SetValue( int32_t a_Index, const uint32_t* a_Values, size_t a_Count = 1 ) const override;
	bool SetValue( int32_t a_Index, const float*    a_Values, size_t a_Count = 1 ) const override;

private:
	
	GLuint                m_ShaderProgramHandle;
	std::vector< GLuint > m_UniformLocations;
};