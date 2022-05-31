#pragma once
#include <stdint.h>
#include "Mesh.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include "RenderMode.hpp"

struct RenderState
{
	const Mesh*     ActiveMesh;
	const Material* ActiveMaterial;
	Matrix4         ActiveModel;
	uint8_t         ActiveRenderTarget;
	uint8_t         ActiveInputBuffer : 3;
	uint8_t         ActiveOutputBuffer : 3;
	RenderMode      ActiveRenderMode : 2;
	bool            AlphaBlendingActive : 1;
	bool            ColourInterpolationActive : 1;
	bool            PositionInterpolationActive : 1;
	bool            NormalInterpolationActive : 1;
	bool            TexelInterpolationActive : 1;
	bool            TangentInterpolationActive : 1;
	bool            BitangentInterpolationActive : 1;
	bool            FrontFaceCullingActive : 1;
	bool            BackFaceCullingActive : 1;
	bool            DepthTestingActive : 1;
	bool            ClippingActive : 1;
};