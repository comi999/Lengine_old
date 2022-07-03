#include "Shader.hpp"

DefineShader( Default_Vertex )
{
	Uniform( Matrix4, u_PVM );
	Attribute( 0, Vector3, a_Position );
	Rendering::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
}

DefineShader( Default_Fragment )
{
	//Uniform( Sampler2D, texture_diffuse );
	//Rendering::FragColour = Rendering::Sample( texture_diffuse, Vector2::One * 0.5f );
	Rendering::FragColour = Vector4::One;
}

Shader Shader::Default;
Shader Shader::Phong;

