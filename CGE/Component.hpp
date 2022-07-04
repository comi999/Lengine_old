#pragma once
#include "entt/entt.hpp"
#include "Utilities.hpp"
#include "File.hpp"
#include "TypeMap.hpp"

typedef uint32_t GameObjectID;
class GameObject;

template < typename T >
class ITransform;

typedef ITransform< void > Transform;

template < typename T >
class IAlias;

typedef IAlias< void > Alias;

class ComponentBase
{
private:

	friend class GameObject;
	friend class Prefab;
	template < typename > friend class IComponent;

	static TypeMap& GetTypeMap()
	{
		static TypeMap TypeMap;
		return TypeMap;
	}

	static entt::registry& GetRegistry()
	{
		static entt::registry Registry;
		return Registry;
	}

	GameObjectID m_ID;
};

namespace Internal
{
	template < typename T >
	constexpr int ForceInitialization()
	{
		static std::vector< T > Static;
		return 0;
	}

	template < Hash _Hash >
	int ComponentHash;
};

#define DefineComponent( Name, Base ) \
template < typename T > \
class I##Name; \
typedef I##Name< void > ##Name; \
template <> \
int Internal::ComponentHash< #Name##_H > = Internal::ForceInitialization< ##Name##* >(); \
template < typename T > \
class I##Name : public I##Base< I##Name< T > >

template < typename T >
class IComponent;

typedef IComponent< void > Component;

template < typename T >
class IComponent : public ComponentBase
{
public:

	inline GameObject* GetOwner()
	{
		return reinterpret_cast< GameObject* >( &m_ID );
	}

	inline const GameObject& GetOwner() const
	{
		return reinterpret_cast< const GameObject& >( m_ID );
	}

	template < typename T >
	static T* AddComponent( GameObjectID a_ID )
	{
		if ( a_ID == static_cast< GameObjectID >( -1 ) )
		{
			return nullptr;
		}

		if ( ComponentBase::GetRegistry().all_of< T >( entt::entity( a_ID ) ) )
		{
			return GetExactComponent< T >( a_ID );
		}

		auto& NewComponent = ComponentBase::GetRegistry().emplace< T >( entt::entity( a_ID ) );
		NewComponent.m_ID = a_ID;
		return &NewComponent;
	}

	template < typename T >
	static T* GetComponent( GameObjectID a_ID )
	{
		if ( a_ID == static_cast< GameObjectID >( -1 ) )
		{
			return nullptr;
		}

		T* Found = ComponentBase::GetRegistry().try_get< T >( entt::entity( a_ID ) );

		if ( Found )
		{
			return Found;
		}

		auto Begin = ComponentBase::GetTypeMap().GetBFBegin< std::remove_const_t< T > >();
		auto End = ComponentBase::GetTypeMap().GetBFEnd();
		++Begin;

		for ( ; Begin != End; ++Begin )
		{
			T* Found = nullptr;
			ComponentBase::GetTypeMap().InvokeFunction( Begin->hash_code(), "GetComponent"_H, Found, a_ID );

			if ( Found )
			{
				return Found;
			}
		}

		return nullptr;
	}

	template < typename T >
	static T* GetExactComponent( GameObjectID a_ID )
	{
		if ( a_ID == static_cast< GameObjectID >( -1 ) )
		{
			return nullptr;
		}

		return ComponentBase::GetRegistry().try_get< T >( entt::entity( a_ID ) );
	}

	template < typename T >
	static bool DestroyComponent( GameObjectID a_ID )
	{
		if ( a_ID == static_cast< GameObjectID >( -1 ) )
		{
			return false;
		}

		return ComponentBase::GetRegistry().remove_if_exists< T >( a_ID );
	}

	inline Transform* GetTransform()
	{
		return GetExactComponent< Transform >( m_ID );
	}

	inline const Transform* GetTransform() const
	{
		return GetExactComponent< Transform >( m_ID );
	}

	inline Alias* GetAlias()
	{
		return GetExactComponent< Alias >( m_ID );
	}

	inline const Alias* GetAlias() const
	{
		return GetExactComponent< Alias >( m_ID );
	}

	template < typename T >
	static std::list< T* > GetComponents()
	{
		std::list< T* > Collated;

		for ( auto Begin = ComponentBase::GetTypeMap().GetBFBegin< T >(), End = ComponentBase::GetTypeMap().GetBFEnd(); Begin != End; ++Begin )
		{
			size_t HashCode = Begin->hash_code();
			std::list< T* > Components;
			ComponentBase::GetTypeMap().InvokeFunction( HashCode, "GetComponents"_H, Components );
			Collated.splice( Collated.end(), Components );
		}

		return Collated;
	}

	template < typename T >
	static std::list< T* > GetExactComponents()
	{
		auto View = ComponentBase::GetRegistry().view< T >();
		std::list< T* > Collated;

		for ( auto Begin = View.rbegin(), End = View.rend(); Begin != End; ++Begin )
		{
			Collated.push_back( &View.get< T >( *Begin ) );
		}

		return Collated;
	}

private:

	friend class GameObject;

	using InheritanceTrace = typename unwrap< IComponent< T > >::Tuple;
	using LeadingType = std::tuple_element_t< std::tuple_size_v< InheritanceTrace > -1, InheritanceTrace >;

public:

	//using Super = get_super< InheritanceTrace >;

private:

	template < typename _Serializer, typename _Component >
	static bool SerializeComponent( _Serializer& a_Serializer, const void* a_Component )
	{
		a_Serializer << *reinterpret_cast< const _Component* >( a_Component );
		return true;
	}

	template < typename _Deserializer, typename _Component >
	static bool DeserializeComponent( _Deserializer& a_Deserializer, void*& a_Component )
	{
		auto cn = typeid( _Component ).name();
		a_Deserializer >> *reinterpret_cast< _Component* >( a_Component );
		_Component* c = reinterpret_cast< _Component* >( a_Component );
		return true;
	}

	static void Setup()
	{
		auto n = typeid( LeadingType ).name();
		ComponentBase::GetTypeMap().RegisterTypes< InheritanceTrace >();
		ComponentBase::GetTypeMap().RegisterFunction< LeadingType >( "AddComponent"_H, AddComponent< LeadingType > );
		ComponentBase::GetTypeMap().RegisterFunction< LeadingType >( "BufferSerializeComponent"_H,   SerializeComponent  < BufferSerializer,   LeadingType > );
		ComponentBase::GetTypeMap().RegisterFunction< LeadingType >( "BufferDeserializeComponent"_H, DeserializeComponent< BufferDeserializer, LeadingType > );
		ComponentBase::GetTypeMap().RegisterFunction< LeadingType >( "GetComponent"_H,  GetExactComponent < LeadingType > );
		ComponentBase::GetTypeMap().RegisterFunction< LeadingType >( "GetComponents"_H, GetExactComponents< LeadingType > );
	}

	inline static OnStart s_Setup = Setup;
};

// Fake components just for testing.
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

template < typename T >
class IImage : public IGraphic< IImage< T > >
{ };

typedef IImage< void > Image;

template < typename T >
class ISprite : public IGraphic< ISprite< T > >
{ };

typedef ISprite< void > Sprite;

template < typename T >
class IAnimatedTile : public IGraphic< IAnimatedTile< T > >
{ };

typedef IAnimatedTile< void > AnimatedTile;