#pragma once
#include "entt/entt.hpp"
#include "TypeMap.hpp"
#include "Component.hpp"
#include "Name.hpp"

typedef uint32_t GameObjectID;

class GameObject;

class ECS
{
private:

	template < typename T >
	struct HasAwake_0
	{
		template < typename U, typename = std::enable_if_t< std::is_same_v< decltype( &U::Awake ), void( U::* )() > > > static char test( decltype( &U::Awake ) );
		template < typename U > static long test( ... );

		static constexpr bool Value = sizeof( test< T >( 0 ) ) == sizeof( char );
	};

	template < typename T >
	struct HasStart_0
	{
		template < typename U, typename = std::enable_if_t< std::is_same_v< decltype( &U::Start ), void( U::* )() > > > static char test( decltype( &U::Start ) );
		template < typename U > static long test( ... );

		static constexpr bool Value = sizeof( test< T >( 0 ) ) == sizeof( char );
	};

	template < typename T >
	struct HasUpdate_0
	{
		template < typename U, typename = std::enable_if_t< std::is_same_v< decltype( &U::Update ), void( U::* )( ) > > > static char test( decltype( &U::Update ) );
		template < typename U > static long test( ... );

		static constexpr bool Value = sizeof( test< T >( 0 ) ) == sizeof( char );
	};

	static GameObjectID Generate()
	{
		auto New = s_Registry.create();
		return reinterpret_cast< GameObjectID& >( New );
	}

public:

	inline static GameObjectID Instantiate()
	{
		auto New = Generate();
		AttachAlias( New, ""_N );
		AttachTransform( New, GameObjectID( -1 ) );
		return New;
	}

	inline static GameObjectID Instantiate( const Name& a_Name )
	{
		auto New = Generate();
		AttachAlias( New, a_Name );
		AttachTransform( New, GameObjectID( -1 ) );
		return New;
	}

	inline static GameObjectID Instantiate( GameObjectID a_Parent )
	{
		auto New = Generate();
		AttachAlias( New, ""_N );
		AttachTransform( New, a_Parent );
		return New;
	}

	inline static GameObjectID Instantiate( const Name& a_Name, GameObjectID a_Parent )
	{
		auto New = Generate();
		AttachAlias( New, a_Name );
		AttachTransform( New, a_Parent );
		return New;
	}

	template < typename T >
	static T* AddComponent( GameObjectID a_GameObjectID )
	{
		static bool RegisterTypes = s_TypeMap.RegisterTypes< typename T::InheritanceTrace >();
		static bool RegisterFunction = s_TypeMap.RegisterFunction< T >( "GetComponent"_H, GetExactComponent< T > );
		static bool RegisterOnConstruct = []()
		{
			s_Registry.on_construct< T >().connect< &ComponentOnConstruct< T > >();
			return true;
		}( );

		if ( a_GameObjectID == static_cast< GameObjectID >( -1 ) )
		{
			return nullptr;
		}

		if ( s_Registry.all_of< T >( entt::entity( a_GameObjectID ) ) )
		{
			return nullptr;
		}

		auto& NewComponent = s_Registry.emplace< T >( entt::entity( a_GameObjectID ) );
		NewComponent.m_GameObjectID = a_GameObjectID;
		return &NewComponent;
	}

	template < typename T >
	static T* GetComponent( GameObjectID a_GameObjectID )
	{
		if ( a_GameObjectID == static_cast< GameObjectID >( -1 ) )
		{
			return nullptr;
		}

		T* Found = s_Registry.try_get< T >( entt::entity( a_GameObjectID ) );

		if ( Found )
		{
			return Found;
		}

		auto Begin = s_TypeMap.GetBFBegin< T >(), End = s_TypeMap.GetBFEnd();
		++Begin;

		for ( ; Begin != End; ++Begin )
		{
			size_t HashCode = Begin->hash_code();
			T* Component = nullptr;
			s_TypeMap.InvokeFunction( HashCode, "GetComponent"_H, Component, a_GameObjectID );

			if ( Component )
			{
				return Component;
			}
		}

		return nullptr;
	}

	template < typename T >
	inline static const T* GetComponentConst( GameObjectID a_GameObjectID )
	{
		return GetComponent< T >( a_GameObjectID );
	}

	template < typename T >
	static T* GetExactComponent( GameObjectID a_GameObjectID )
	{
		if ( a_GameObjectID == static_cast< GameObjectID >( -1 ) )
		{
			return nullptr;
		}

		return s_Registry.try_get< T >( entt::entity( a_GameObjectID ) );
	}

	template < typename T >
	inline static const T* GetExactComponentConst( GameObjectID a_GameObjectID )
	{
		return GetComponent< T >( a_GameObjectID );
	}

	template < typename T >
	static bool DestroyComponent( GameObjectID a_GameObjectID )
	{
		if ( a_GameObjectID == static_cast< GameObjectID >( -1 ) )
		{
			return nullptr;
		}

		return false;//return s_Registry.remove_if_exists< T >( a_GameObjectID );
	}

	template < typename T >
	static auto GetAll()
	{
		return s_Registry.view< T >();
	}

private:

	friend class ComponentBase;

	static void AttachTransform( GameObjectID a_GameObjectID, GameObjectID a_Parent );
	static void AttachAlias( GameObjectID a_GameObjectID, const Name& a_Name );

	template < typename T >
	static void ComponentOnConstruct( entt::registry& a_Registry, entt::entity a_Entity )
	{
		reinterpret_cast< ComponentBase& >( a_Registry.get< T >( a_Entity ) ).m_GameObjectID = GameObjectID( a_Entity );
	}

	static TypeMap		  s_TypeMap;
	static entt::registry s_Registry;
};