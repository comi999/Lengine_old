#pragma once
#include "RenderObject.hpp"

struct RenderInstruction
{
	enum class Modification : uint8_t
	{
		SET,
		ENABLE,
		DISABLE,
		DRAW
	};

	Modification Modification;
	RenderObject Object;

	union
	{
		const void* Source;
		uint32_t    Index;
		bool        State;
	};
};