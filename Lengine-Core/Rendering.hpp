#pragma once
#include <cstdint>
#include <string>

#include "Colour.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Math.hpp"

typedef uint32_t BufferHandle;
typedef uint32_t ArrayHandle;
typedef uint32_t TextureHandle;
typedef uint32_t ShaderHandle;

namespace Rendering
{
	void Init();
	void CompileProgram( const std::string& a_Source, ShaderProgramHandle& o_ShaderProgramHandle );
	void DeleteProgram( ShaderProgramHandle a_ShaderProgramHandle );
	void ApplyMesh( const Mesh& a_Mesh );
	void ApplyMaterial( const Material& a_Material );
	void ApplyShader( const Shader& a_Shader );
	void ApplyUniform( const char* a_Name, uint32_t a_Count, const float* a_Value );
	void ApplyUniform( const char* a_Name, uint32_t a_Count, const int32_t* a_Value );
	void ApplyUniform( const char* a_Name, uint32_t a_Count, const uint32_t* a_Value );
	void ApplyUniform( const char* a_Name, uint32_t a_Count, const Matrix4* a_Value );
	void Clear();
	void Draw();
};