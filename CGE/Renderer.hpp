#pragma once
#include "Component.hpp"

template < typename T >
class IRenderer;

typedef IRenderer< void > Renderer;

template < typename T >
class IRenderer : public IComponent< IRenderer< T > >
{

};