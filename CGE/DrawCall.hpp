#pragma once
#include "Mesh.hpp"
#include "Material.hpp"
#include "Math.hpp"
#include "RenderMode.hpp"

struct DrawCall
{
	bool operator <( const DrawCall& a_DrawCall )
	{
		if ( ActiveMesh != a_DrawCall.ActiveMesh )
		{
			return ActiveMesh < a_DrawCall.ActiveMesh;
		}

		if ( ActiveMaterial != a_DrawCall.ActiveMaterial )
		{
			return ActiveMaterial < a_DrawCall.ActiveMaterial;
		}

		if ( ActiveModel != a_DrawCall.ActiveModel )
		{
			return ActiveModel < a_DrawCall.ActiveModel;
		}

		if ( ActiveRenderMode != a_DrawCall.ActiveRenderMode )
		{
			return ActiveRenderMode < a_DrawCall.ActiveRenderMode;
		}

		return true;
	}

	const Mesh*      ActiveMesh;
	const Material*  ActiveMaterial;
	const Matrix4*   ActiveModel;
	RenderMode       ActiveRenderMode : 2;
};