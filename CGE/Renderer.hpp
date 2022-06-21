#pragma once
#include "Component.hpp"
#include "RenderQueue.hpp"

template < typename T >
class IRenderer;

typedef IRenderer< void > Renderer;

template < typename T >
class IRenderer : public IComponent< IRenderer< T > >
{
private:

	friend class RenderPipeline;
	
	virtual void OnDraw( RenderQueue ) { };
};