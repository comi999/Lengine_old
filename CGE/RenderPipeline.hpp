#pragma once
#include <list>
#include "ECS.hpp"
#include "Mesh.hpp"
#include "Frustum.hpp"
#include "Renderer.hpp"
#include "Rendering.hpp"
#include "RenderInstruction.hpp"
#include "RenderQueue.hpp"

class RenderPipeline
{
private:

	static void Run()
	{
		// Collect all instructions.
		auto Renderers = ECS::GetComponents< Renderer >();
		RenderQueue Queue;

		for ( auto Renderer : Renderers )
		{
			Renderer->OnDraw( Queue );
		}

		// Sort all render instructions

		//s_RenderInstructions.sort();


	}
};