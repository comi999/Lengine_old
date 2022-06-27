#include "Shader.hpp"

DefineShader( Default_Vertex )
{
	Uniform( Matrix4, u_PVM );
	Attribute( 0, Vector3, a_Position );
	Rendering::Position = Math::Multiply( u_PVM, Vector4( a_Position, 1.0f ) );
}

DefineShader( Default_Fragment )
{
	Rendering::FragColour = Vector4::One;
}

Shader Shader::Default;
Shader Shader::Phong;

