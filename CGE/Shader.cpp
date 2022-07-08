#include "Shader.hpp"

// Vertex Default
DefineShader( Vertex_Default )
{
	Uniform( Matrix4, u_PVM );
	Attribute( 0, Vector3, a_Position );
	Rendering::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
}

// Vertex Texel Only
DefineShader( Vertex_Texel )
{
	Uniform( Matrix4, u_PVM );
	Attribute( 0, Vector3, a_Position );
	Attribute( 1, Vector2, a_Texel );
	Varying_Out( Vector2, Texel );

	Rendering::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
	Texel = a_Texel;
}

// Fragment Default
DefineShader( Fragment_Default )
{
	Rendering::FragColour = Colour::PINK;
}

// Fragment Diffuse
DefineShader( Fragment_Diffuse )
{
	Uniform( Sampler2D, texture_diffuse );
	Varying_In( Vector2, Texel );

	Rendering::FragColour = Rendering::Sample( texture_diffuse, Texel );
}

// Fragment Specular
DefineShader( Fragment_Specular )
{
	Uniform( Sampler2D, texture_specular );
	Varying_In( Vector2, Texel );

	Rendering::FragColour = Rendering::Sample( texture_specular, Texel );
}

// Fragment Normal
DefineShader( Fragment_Normal )
{
	Uniform( Sampler2D, texture_normal );
	Varying_In( Vector2, Texel );

	Rendering::FragColour = Rendering::Sample( texture_normal, Texel );
}

Shader Shader::Default;
Shader Shader::Diffuse  = Shader( "Diffuse"_N,  "Vertex_Texel", "Fragment_Diffuse"  );
Shader Shader::Specular = Shader( "Specular"_N, "Vertex_Texel", "Fragment_Specular" );
Shader Shader::Normal  = Shader( "Normal"_N,  "Vertex_Texel", "Fragment_Normal"  );
Shader Shader::Phong;//    = Shader( "Phong"_N,    "Vertex_Texel", "Phong_Fragment"    );

