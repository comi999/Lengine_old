#include "Shader.hpp"

class ConsoleGLShader : public Shader
{
public:

	bool SetInt( const char* a_Name, const int32_t* a_Values, size_t a_Count = 1 ) const override;
	bool SetUInt( const char* a_Name, const uint32_t* a_Values, size_t a_Count = 1 ) const override;
	bool SetFloat( const char* a_Name, const float* a_Values, size_t a_Count = 1 ) const override;
	bool SetMatrix4( const char* a_Name, const Matrix4* a_Values, size_t a_Count = 1 ) const override;
};