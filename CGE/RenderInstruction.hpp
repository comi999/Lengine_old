#pragma once
#include <cstdint>

struct RenderInstruction
{
	enum class Modification : uint8_t
	{
		NONE,
		SET,
		ENABLE,
		DISABLE,
		STATIC_DRAW
	};

	enum class Object : uint8_t
	{
		Mesh,
		Material,
		Model,
		Buffer,
		RenderMode,
		AlphaBlending,
		FrontFaceCulling,
		BackFaceCulling,
		DepthTest,
		Clipping,
	};

	Modification Modification = Modification::NONE;
	Object Object;

	union
	{
		const void* Source;
		uint32_t    Index;
		bool        State;
	};
};

// Material
// - Shader 

class DrawCall
{
	// Things I need to do to set up with rendering api.
	//
	// I need to set the mesh data.
	// From the material, I will know how to set up textures, constants, and the shader that should be used.
	//
};