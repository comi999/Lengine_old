#pragma once
#include "ECS.hpp"

typedef uint32_t GameObjectID;
class GameObject;

class ComponentBase
{
protected:

	friend class ECS;

	GameObjectID m_GameObjectID;
	bool         m_StartRun : 1;
	bool         m_Ticking  : 1;
};

template < typename T >
class IComponent : public ComponentBase
{
private:

	template < typename... T >
	struct combine_tuple
	{
	};

	template < typename... T, typename... U >
	struct combine_tuple< std::tuple< T... >, std::tuple< U... > >
	{
		using Tuple = std::tuple< T..., U... >;
	};

	template < typename T >
	struct unwrappable : public std::false_type
	{
	};

	template < template < typename > class T, typename U >
	struct unwrappable< T< U > > : public std::true_type
	{
	};

	template < typename T >
	struct unwrap
	{
		using Tuple = std::tuple<>;
	};

	template < template < typename... > class T, typename U >
	struct unwrap< T< U > > : public unwrap< U >
	{
		using Tuple = typename combine_tuple< std::tuple< T< void > >, typename unwrap< U >::Tuple >::Tuple;
	};

	template < template < typename... > class T >
	struct unwrap< T< void > >
	{
		using Tuple = std::tuple< T< void > >;
	};

public:

	inline GameObjectID GetOwnerID() const
	{
		return m_GameObjectID;
	}

	inline GameObject& GetOwner()
	{
		return reinterpret_cast< GameObject& >( m_GameObjectID );
	}

	inline const GameObject& GetOwner() const
	{
		return reinterpret_cast< const GameObject& >( m_GameObjectID );
	}

	using InheritanceTrace = typename unwrap< IComponent< T > >::Tuple;
	using LeadingType = std::tuple_element_t< std::tuple_size_v< InheritanceTrace > - 1, InheritanceTrace >;
};

typedef IComponent< void > Component;

template < typename T >
class IGraphic : public IComponent< IGraphic< T > >
{

};

typedef IGraphic< void > Graphic;

template < typename T >
class IInteractable : public IGraphic< IInteractable< T > >
{

};

typedef IInteractable< void > Interactable;

template < typename T >
class IButton : public IInteractable< IButton< T > >
{

};

typedef IButton< void > Button;