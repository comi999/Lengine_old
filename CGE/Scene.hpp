#pragma once
#include "Transform.hpp"

class Scene
{
public:

	static void Tick()
	{
		auto AllTransforms = Component::GetExactComponents< Transform >();

		for ( auto Transform : AllTransforms )
		{
			Transform->UpdateTransform();
		}
	}

};