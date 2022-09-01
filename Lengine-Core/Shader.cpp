#include "Shader.hpp"
#include "Light.hpp"
#include "ConsoleGL.hpp"

Shader Shader::Default;
Shader Shader::Diffuse =
Shader( "Diffuse"_N, "API: OpenGL\n"
		"VERTEX:\n"
		"#version 340 core\n"
		"layout(location=0) in vec3 position;\n"
		"uniform mat4 u_PVM;\n"
		"void main() {\n"
		"gl_Position = vec4(0.0, 0.0, 0.0, 1.0);// u_PVM * vec4(position, 1.0);\n"
		"}\n"
		"FRAGMENT:\n"
		"#version 340 core\n"
		"uniform mat4 u_PVM;\n"
		"out vec4 FragColour;\n"
		"void main() {\n"
		"FragColour = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n" );



//Shader( "Diffuse"_N, "API: ConsoleGL VERTEX: Vertex_Texel FRAGMENT: Fragment_Diffuse" );
Shader Shader::Specular = Shader( "Specular"_N, "API: ConsoleGL VERTEX: Vertex_Texel FRAGMENT: Fragment_Specular" );
Shader Shader::Normal = Shader( "Normal"_N, "API: ConsoleGL VERTEX: Vertex_Texel FRAGMENT: Fragment_Normal" );
Shader Shader::Phong;//    = Shader( "Phong"_N,    "Vertex_Texel", "Phong_Fragment"    );
Shader Shader::UnlitFlatColour = Shader( "UnlitFlatColour"_N, "API: ConsoleGL VERTEX: Vertex_Default FRAGMENT: Fragment_Unlit_Flat_Colour" );
Shader Shader::LitFlatColour = Shader( "LitFlatColour"_N, "API: ConsoleGL VERTEX: Vertex_Lit_Flat_Colour FRAGMENT: Fragment_Lit_Flat_Colour" );
Shader Shader::LitDiffuse = Shader( "LitDiffuse"_N, "API: ConsoleGL VERTEX: Vertex_Lit_Diffuse FRAGMENT: Fragment_Lit_Diffuse" );

Shader Shader::Spotlight = Shader( "Spotlight"_N, "API: ConsoleGL VERTEX: Vertex_Spotlight FRAGMENT: Fragment_Spotlight" );

// Vertex Default
DefineShader( Vertex_Default )
{
	Uniform( Matrix4, u_PVM );
	Attribute( 0, Vector3, a_Position );
	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
}

// Vertex Texel Only
DefineShader( Vertex_Texel )
{
	Uniform( Matrix4, u_PVM );
	Attribute( 0, Vector3, a_Position );
	Attribute( 1, Vector2, a_Texel );
	Varying_Out( Vector2, Texel );
	
	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
	Texel = a_Texel;
}

// Vertex Lit Diffuse
DefineShader( Vertex_Lit_Diffuse )
{
	/*Uniform( Matrix4, u_PVM );
	Attribute( 0, Vector3, a_Position );
	Attribute( 1, Vector2, a_Texel );
	Varying_Out( Vector2, Texel );

	Rendering::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
	Texel = a_Texel;*/

	Uniform( Matrix4, u_PVM );
	Uniform( Matrix4, u_Model );
	Attribute( 0, Vector3, a_Position );
	Attribute( 1, Vector2, a_Texel );
	Attribute( 3, Vector3, a_Normal );
	Varying_Out( Vector3, Normal );
	Varying_Out( Vector2, Texel );
	
	Texel = a_Texel;
	Normal = Math::Multiply( u_Model, Vector4( a_Normal ) );
	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
}

// Fragment Default
DefineShader( Fragment_Default )
{
	ConsoleGL::FragColour = Colour::PINK;
}

// Fragment Diffuse
DefineShader( Fragment_Diffuse )
{
	Uniform( ConsoleGL::Sampler2D, texture_diffuse );
	Varying_In( Vector2, Texel );

	ConsoleGL::FragColour = ConsoleGL::Sample( texture_diffuse, Texel ) * Vector4::One * 0.7f;
}

// Fragment Lit Diffuse
DefineShader( Fragment_Lit_Diffuse )
{
	Uniform( Vector3, u_SunDirection0 );
	Uniform( Vector3, u_SunAmbient0 );
	Uniform( ConsoleGL::Sampler2D, texture_diffuse );
	Varying_In( Vector3, Normal );
	Varying_In( Vector2, Texel );

	Vector4 diffuse_colour = ConsoleGL::Sample( texture_diffuse, Texel );

	float Intensity = Math::Clamp( Math::Dot( -u_SunDirection0, Normal ), 0.5f, 1.0f );
	ConsoleGL::FragColour.x = Intensity * diffuse_colour.x * u_SunAmbient0.x;
	ConsoleGL::FragColour.y = Intensity * diffuse_colour.y * u_SunAmbient0.y;
	ConsoleGL::FragColour.z = Intensity * diffuse_colour.z * u_SunAmbient0.z;
	ConsoleGL::FragColour.w = diffuse_colour.w;
}

// Fragment Specular
DefineShader( Fragment_Specular )
{
	Uniform( ConsoleGL::Sampler2D, texture_specular );
	Varying_In( Vector2, Texel );

	ConsoleGL::FragColour = ConsoleGL::Sample( texture_specular, Texel );
}

// Fragment Normal
DefineShader( Fragment_Normal )
{
	Uniform( ConsoleGL::Sampler2D, texture_normal );
	Varying_In( Vector2, Texel );

	ConsoleGL::FragColour = ConsoleGL::Sample( texture_normal, Texel );
}

// Vertex Unlit Flat Colour
//DefineShader( Vertex_Unlit_Flat_Colour )
//{
//
//}

// Fragment Unlit Flat Colour
DefineShader( Fragment_Unlit_Flat_Colour )
{
	Uniform( Vector4, diffuse_colour );

	ConsoleGL::FragColour = diffuse_colour;
}

// Vertex Lit Flat Colour
DefineShader( Vertex_Lit_Flat_Colour )
{
	Uniform( Matrix4, u_PVM );
	Uniform( Matrix4, u_Model );
	Attribute( 0, Vector3, a_Position );
	Attribute( 3, Vector3, a_Normal );
	Varying_Out( Vector3, Normal );

	Normal = Math::Multiply( u_Model, Vector4( a_Normal ) );
	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
}

// Fragment Lit Flat Colour
DefineShader( Fragment_Lit_Flat_Colour )
{
	Uniform( Vector4, diffuse_colour );
	Uniform( Vector3, u_SunDirection0 );
	Uniform( Vector3, u_SunAmbient0 );
	Varying_In( Vector3, Normal );

	float Intensity = Math::Clamp( -Math::Dot( u_SunDirection0, Normal ), 0.3f, 1.0f );
	ConsoleGL::FragColour.x = Intensity * diffuse_colour.x * u_SunAmbient0.x;
	ConsoleGL::FragColour.y = Intensity * diffuse_colour.y * u_SunAmbient0.y;
	ConsoleGL::FragColour.z = Intensity * diffuse_colour.z * u_SunAmbient0.z;
	ConsoleGL::FragColour.w = diffuse_colour.w;
}


// Vertex Spotlight shader
DefineShader( Vertex_Spotlight )
{
	Uniform( Matrix4, u_PVM );
	Uniform( Matrix4, u_Model );
	Attribute( 0, Vector3, a_Position );
	Attribute( 1, Vector2, a_Texel );
	Attribute( 3, Vector3, a_Normal );
	
	Varying_Out( Vector3, Pos );
	Varying_Out( Vector2, Texel );
	Varying_Out( Vector3, Normal );
	
	Pos = Math::Multiply( u_Model, Vector4( a_Position, 1.0f ) );
	Texel = a_Texel;
	Normal = Math::Multiply( u_Model, Vector4( a_Normal ) );
	ConsoleGL::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
}

// Fragment Spotlightshader
DefineShader( Fragment_Spotlight )
{
	Uniform( ConsoleGL::Sampler2D, texture_diffuse );

	// Using this as spotlight dir
	Uniform( Vector3, u_SunDirection0 );

	// Using this as spotlight pos
	Uniform( Vector3, u_SunAmbient0 );
	
	Varying_In( Vector3, Pos );
	Varying_In( Vector2, Texel );
	Varying_In( Vector3, Normal );

	Vector4 fragment = ConsoleGL::Sample( texture_diffuse, Texel );

	Vector3 FragToLight = Math::Normalize( Pos - u_SunAmbient0 );
	float Intensity = Math::Dot( FragToLight, u_SunDirection0 );
	Intensity = Math::Pow( Intensity, 2.0f );

	//float Intensity = Math::Clamp( -Math::Dot( u_SunDirection0, Normal ), 0.3f, 1.0f );
	ConsoleGL::FragColour.x = Intensity * fragment.x;
	ConsoleGL::FragColour.y = Intensity * fragment.y;
	ConsoleGL::FragColour.z = Intensity * fragment.z;
	ConsoleGL::FragColour.w = 1.0f;
}