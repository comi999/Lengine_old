#pragma once
#include "Component.hpp"
#include "RenderQueue.hpp"

DefineComponent( Renderer, Component )
{
public:

	friend class RenderPipeline;
	
	virtual void OnRender( RenderQueue& a_RenderQueue ) const { };
};