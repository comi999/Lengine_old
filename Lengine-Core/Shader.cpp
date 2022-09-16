#include <regex>

#include "Shader.hpp"
#include "Pointer.hpp"
#include "Implementation/Shader/ConsoleGLShader.hpp"
#include "Implementation/Shader/OpenGLShader.hpp"

bool ShaderInput::operator==( const ShaderInput& a_ShaderInput ) const
{
	return
		m_Shader == a_ShaderInput.m_Shader &&
		m_Index == a_ShaderInput.m_Index;
}

ShaderIterator ShaderIterator::operator++( int )
{
	ShaderIterator Temp = *this;
	++*this;
	return Temp;
}

Shared< Shader > Shader::Create( const std::string& a_Source )
{
	// This is a placeholder way to process a shader source that is structured like the following:
	// API: OpenGL
	// 
	// VERTEX_BEGIN
	// #version 330
	// layout ( location = 0 ) in vec4 i_Position;
	// uniform mat4 u_PVM;
	// 
	// void main()
	// {
	//     gl_Position = u_PVM * i_Position;
	// }
	// VERTEX_END
	// FRAGMENT_BEGIN
	// #version 330
	// uniform vec4 u_Colour;
	// out vec4 FragColour;
	// 
	// void main()
	// {
	//     FragColour = u_Colour;
	// }
	// FRAGMENT_END

	std::regex Regex( "API:\\s*(ConsoleGL|OpenGL)" );
	std::smatch Match;

	if ( !std::regex_search( a_Source, Match, Regex ) )
	{
		return nullptr;
	}

	Shared< Shader > Loaded;

	switch ( CRC32_RT( Match[ 1 ].str().c_str() ) )
	{
		case "ConsoleGL"_H: Loaded = std::make_shared< ConsoleGLShader >(); break;
		case "OpenGL"_H:    Loaded = std::make_shared< OpenGLShader    >(); break;
		case "Vulkan"_H:    Loaded;// Shared< >( new ); break;
	}

	if ( !Loaded.get() )
	{
		return nullptr;
	}

	Loaded->m_P = nullptr;
	Loaded->m_V = nullptr;
	Loaded->m_M = nullptr;
	Loaded->m_PV = nullptr;
	Loaded->m_VM = nullptr;
	Loaded->m_PVM = nullptr;
	Loaded->OnCreate();
	return Loaded;
}

int32_t Shader::GetIndex( Hash a_Name ) const
{
	auto Iter = std::search( m_Inputs.begin(), m_Inputs.end(), [&]( const ShaderInput& a_Input ) { a_Input.GetName().HashCode() == a_Name; } );
	return Iter == m_Inputs.end() ? -1 : Iter - m_Inputs.begin();
}

bool Shader::SetPVM( const Matrix4& a_P, const Matrix4& a_V, const Matrix4& a_M ) const
{
	if ( m_P ) m_P->SetValue( a_P.Data, 16 );
	if ( m_V ) m_V->SetValue( a_V.Data, 16 );
	if ( m_M ) m_M->SetValue( a_M.Data, 16 );

	auto PV = Math::Multiply( a_P, a_V );
	if ( m_PV ) m_PV->SetValue( PV.Data, 16 );
	if ( m_VM ) m_VM->SetValue( Math::Multiply( a_V, a_M ).Data, 16 );
	if ( m_PVM ) m_PVM->SetValue( Math::Multiply( PV, a_M ).Data, 16 );
}

bool Shader::Compile()
{
	OnCompile();
}

bool Shader::Decompile()
{
	OnDecompile();
	m_Inputs.clear();
}

void Shader::AddShaderInput( const Name& a_Name ) const
{
	const_cast< Shader* >( this )->m_Inputs.emplace_back( *this, m_Inputs.size(), a_Name );

	switch ( a_Name.HashCode() )
	{
		case "u_P"_H:   const_cast< Shader* >( this )->m_P   = &m_Inputs.back(); break;
		case "u_V"_H:   const_cast< Shader* >( this )->m_V   = &m_Inputs.back(); break;
		case "u_M"_H:   const_cast< Shader* >( this )->m_M   = &m_Inputs.back(); break;
		case "u_PV"_H:  const_cast< Shader* >( this )->m_PV  = &m_Inputs.back(); break;
		case "u_VM"_H:  const_cast< Shader* >( this )->m_VM  = &m_Inputs.back(); break;
		case "u_PVM"_H: const_cast< Shader* >( this )->m_PVM = &m_Inputs.back(); break;
		default: break;
	}
}






































//Shader Shader::Default;
//Shader Shader::Diffuse =
////Shader( "Diffuse"_N, "API: OpenGL\n"
////		"VERTEX:\n"
////		"#version 340 core\n"
////		"layout(location=0) in vec3 position;\n"
////		"uniform mat4 u_PVM;\n"
////		"void main() {\n"
////		"gl_Position = vec4(0.0, 0.0, 0.0, 1.0);// u_PVM * vec4(position, 1.0);\n"
////		"}\n"
////		"FRAGMENT:\n"
////		"#version 340 core\n"
////		"uniform mat4 u_PVM;\n"
////		"out vec4 FragColour;\n"
////		"void main() {\n"
////		"FragColour = vec4(1.0, 0.0, 0.0, 1.0);\n"
////		"}\n" );
//
//
//
//Shader( "Diffuse"_N, "API: ConsoleGL VERTEX: Vertex_Texel FRAGMENT: Fragment_Diffuse" );
//Shader Shader::Specular = Shader( "Specular"_N, "API: ConsoleGL VERTEX: Vertex_Texel FRAGMENT: Fragment_Specular" );
//Shader Shader::Normal = Shader( "Normal"_N, "API: ConsoleGL VERTEX: Vertex_Texel FRAGMENT: Fragment_Normal" );
//Shader Shader::Phong;//    = Shader( "Phong"_N,    "Vertex_Texel", "Phong_Fragment"    );
//Shader Shader::UnlitFlatColour = Shader( "UnlitFlatColour"_N, "API: ConsoleGL VERTEX: Vertex_Default FRAGMENT: Fragment_Unlit_Flat_Colour" );
//Shader Shader::LitFlatColour = Shader( "LitFlatColour"_N, "API: ConsoleGL VERTEX: Vertex_Lit_Flat_Colour FRAGMENT: Fragment_Lit_Flat_Colour" );
//Shader Shader::LitDiffuse = Shader( "LitDiffuse"_N, "API: ConsoleGL VERTEX: Vertex_Lit_Diffuse FRAGMENT: Fragment_Lit_Diffuse" );
//
//Shader Shader::Spotlight = Shader( "Spotlight"_N, "API: ConsoleGL VERTEX: Vertex_Spotlight FRAGMENT: Fragment_Spotlight" );
//
//// Vertex Default
//DefineShader( Vertex_Default )
//{
//	Uniform( Matrix4, u_PVM );
//	Attribute( 0, Vector3, a_Position );
//	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
//}
//
//// Vertex Texel Only
//DefineShader( Vertex_Texel )
//{
//	Uniform( Matrix4, u_PVM );
//	Attribute( 0, Vector3, a_Position );
//	Attribute( 1, Vector2, a_Texel );
//	Varying_Out( Vector2, Texel );
//	
//	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
//	Texel = a_Texel;
//}
//
//// Vertex Lit Diffuse
//DefineShader( Vertex_Lit_Diffuse )
//{
//	/*Uniform( Matrix4, u_PVM );
//	Attribute( 0, Vector3, a_Position );
//	Attribute( 1, Vector2, a_Texel );
//	Varying_Out( Vector2, Texel );
//
//	Rendering::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
//	Texel = a_Texel;*/
//
//	Uniform( Matrix4, u_PVM );
//	Uniform( Matrix4, u_Model );
//	Attribute( 0, Vector3, a_Position );
//	Attribute( 1, Vector2, a_Texel );
//	Attribute( 3, Vector3, a_Normal );
//	Varying_Out( Vector3, Normal );
//	Varying_Out( Vector2, Texel );
//	
//	Texel = a_Texel;
//	Normal = Math::Multiply( u_Model, Vector4( a_Normal ) );
//	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
//}
//
//// Fragment Default
//DefineShader( Fragment_Default )
//{
//	ConsoleGL::FragColour = Colour::PINK;
//}
//
//// Fragment Diffuse
//DefineShader( Fragment_Diffuse )
//{
//	Uniform( ConsoleGL::Sampler2D, texture_diffuse );
//	Varying_In( Vector2, Texel );
//
//	ConsoleGL::FragColour = ConsoleGL::Sample( texture_diffuse, Texel ) * Vector4::One * 0.7f;
//}
//
//// Fragment Lit Diffuse
//DefineShader( Fragment_Lit_Diffuse )
//{
//	Uniform( Vector3, u_SunDirection0 );
//	Uniform( Vector3, u_SunAmbient0 );
//	Uniform( ConsoleGL::Sampler2D, texture_diffuse );
//	Varying_In( Vector3, Normal );
//	Varying_In( Vector2, Texel );
//
//	Vector4 diffuse_colour = ConsoleGL::Sample( texture_diffuse, Texel );
//
//	float Intensity = Math::Clamp( Math::Dot( -u_SunDirection0, Normal ), 0.5f, 1.0f );
//	ConsoleGL::FragColour.x = Intensity * diffuse_colour.x * u_SunAmbient0.x;
//	ConsoleGL::FragColour.y = Intensity * diffuse_colour.y * u_SunAmbient0.y;
//	ConsoleGL::FragColour.z = Intensity * diffuse_colour.z * u_SunAmbient0.z;
//	ConsoleGL::FragColour.w = diffuse_colour.w;
//}
//
//// Fragment Specular
//DefineShader( Fragment_Specular )
//{
//	Uniform( ConsoleGL::Sampler2D, texture_specular );
//	Varying_In( Vector2, Texel );
//
//	ConsoleGL::FragColour = ConsoleGL::Sample( texture_specular, Texel );
//}
//
//// Fragment Normal
//DefineShader( Fragment_Normal )
//{
//	Uniform( ConsoleGL::Sampler2D, texture_normal );
//	Varying_In( Vector2, Texel );
//
//	ConsoleGL::FragColour = ConsoleGL::Sample( texture_normal, Texel );
//}
//
//// Vertex Unlit Flat Colour
////DefineShader( Vertex_Unlit_Flat_Colour )
////{
////
////}
//
//// Fragment Unlit Flat Colour
//DefineShader( Fragment_Unlit_Flat_Colour )
//{
//	Uniform( Vector4, diffuse_colour );
//
//	ConsoleGL::FragColour = diffuse_colour;
//}
//
//// Vertex Lit Flat Colour
//DefineShader( Vertex_Lit_Flat_Colour )
//{
//	Uniform( Matrix4, u_PVM );
//	Uniform( Matrix4, u_Model );
//	Attribute( 0, Vector3, a_Position );
//	Attribute( 3, Vector3, a_Normal );
//	Varying_Out( Vector3, Normal );
//
//	Normal = Math::Multiply( u_Model, Vector4( a_Normal ) );
//	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
//}
//
//// Fragment Lit Flat Colour
//DefineShader( Fragment_Lit_Flat_Colour )
//{
//	Uniform( Vector4, diffuse_colour );
//	Uniform( Vector3, u_SunDirection0 );
//	Uniform( Vector3, u_SunAmbient0 );
//	Varying_In( Vector3, Normal );
//
//	float Intensity = Math::Clamp( -Math::Dot( u_SunDirection0, Normal ), 0.3f, 1.0f );
//	ConsoleGL::FragColour.x = Intensity * diffuse_colour.x * u_SunAmbient0.x;
//	ConsoleGL::FragColour.y = Intensity * diffuse_colour.y * u_SunAmbient0.y;
//	ConsoleGL::FragColour.z = Intensity * diffuse_colour.z * u_SunAmbient0.z;
//	ConsoleGL::FragColour.w = diffuse_colour.w;
//}
//
//
//// Vertex Spotlight shader
//DefineShader( Vertex_Spotlight )
//{
//	Uniform( Matrix4, u_PVM );
//	Uniform( Matrix4, u_Model );
//	Attribute( 0, Vector3, a_Position );
//	Attribute( 1, Vector2, a_Texel );
//	Attribute( 3, Vector3, a_Normal );
//	
//	Varying_Out( Vector3, Pos );
//	Varying_Out( Vector2, Texel );
//	Varying_Out( Vector3, Normal );
//	
//	Pos = Math::Multiply( u_Model, Vector4( a_Position, 1.0f ) );
//	Texel = a_Texel;
//	Normal = Math::Multiply( u_Model, Vector4( a_Normal ) );
//	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
//}
//
//// Fragment Spotlightshader
//DefineShader( Fragment_Spotlight )
//{
//	Uniform( ConsoleGL::Sampler2D, texture_diffuse );
//
//	// Using this as spotlight dir
//	Uniform( Vector3, u_SunDirection0 );
//
//	// Using this as spotlight pos
//	Uniform( Vector3, u_SunAmbient0 );
//	
//	Varying_In( Vector3, Pos );
//	Varying_In( Vector2, Texel );
//	Varying_In( Vector3, Normal );
//
//	Vector4 fragment = ConsoleGL::Sample( texture_diffuse, Texel );
//
//	Vector3 FragToLight = Math::Normalize( Pos - u_SunAmbient0 );
//	float Intensity = Math::Dot( FragToLight, u_SunDirection0 );
//	Intensity = Math::Pow( Intensity, 2.0f );
//
//	//float Intensity = Math::Clamp( -Math::Dot( u_SunDirection0, Normal ), 0.3f, 1.0f );
//	ConsoleGL::FragColour.x = Intensity * fragment.x;
//	ConsoleGL::FragColour.y = Intensity * fragment.y;
//	ConsoleGL::FragColour.z = Intensity * fragment.z;
//	ConsoleGL::FragColour.w = 1.0f;
//}