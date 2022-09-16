#include <vector>

#include "../../Shader.hpp"
#include "../../ConsoleGL.hpp"

class ConsoleGLShader : public Shader
{
public:
	
	RenderingAPI GetAPI() const override { return RenderingAPI::ConsoleGL; }
	bool IsCompiled() const override;
	bool Compile() const override;
	bool Decompile() const override;
	int32_t GetIndex( const char* a_Name ) const override;
	bool SetInt( int32_t a_Index, const int32_t* a_Values, size_t a_Count = 1 ) const override;
	bool SetUInt( int32_t a_Index, const uint32_t* a_Values, size_t a_Count = 1 ) const override;
	bool SetFloat( int32_t a_Index, const float* a_Values, size_t a_Count = 1 ) const override;
	bool SetMatrix4( int32_t a_Index, const Matrix4* a_Values, size_t a_Count = 1 ) const override;

private:

	struct UniformInfo
	{
		std::string Name;
		int32_t     Location;
	};

	ShaderProgramHandle        m_ShaderProgramHandle;
	std::vector< UniformInfo > m_UniformInfos;
};