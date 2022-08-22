#include "Material.hpp"

Material Material::UnlitFlatColour = []()
{
	Material Temp;
	Temp.SetName( "UnlitFlatColour"_N );
	Temp.AddProperty( "diffuse_colour"_N, ( Vector4 )Colour::WHITE );
	Temp.SetShader( Shader::UnlitFlatColour );
	return Temp;
}();
Material Material::LitFlatColour = []()
{
	Material Temp;
	Temp.SetName( "LitFlatColour"_N );
	Temp.AddProperty( "diffuse_colour"_N, ( Vector4 )Colour::WHITE );
	Temp.SetShader( Shader::LitFlatColour );
	return Temp;
}();