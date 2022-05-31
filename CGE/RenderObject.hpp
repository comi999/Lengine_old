#pragma once
#include <stdint.h>

enum class RenderObject : uint8_t
{
	Mesh,
	Material,
	Model,
	Buffer,
	RenderMode,
	AlphaBlending,
	ColourInterpolation,
	PositionInterpolation,
	NormalInterpolation,
	TangentInterpolation,
	BitangentInterpolation,
	FrontFaceCulling,
	BackFaceCulling,
	DepthTest,
	Clipping,
};