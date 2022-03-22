#pragma once
#include "entt/entt.hpp"
#include "TypeMap.hpp"
#include "Component.hpp"

typedef entt::entity GameObjectID;

class GameObject;

class ECS
{
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

	inline static GameObjectID GenerateGameObjectID()
	{
		return GameObjectID( s_Registry.create() );
	}

public:

	template < typename T >
	static T* AddComponent( GameObject& a_GameObject, GameObjectID a_GameObjectID )
	{
		static bool Registered = 
			s_TypeMap.RegisterTypes< typename T::InheritanceTrace >() || 
			s_TypeMap.RegisterFunction< T >( "GetComponent"_H, GetComponentImpl< T > );

		if ( s_Registry.all_of< T >( a_GameObjectID ) )
		{
			return nullptr;
		}

		auto& NewComponent = s_Registry.emplace< T >( a_GameObjectID );
		NewComponent.m_GameObject = &a_GameObject;
		return &NewComponent;
	}

	template < typename T >
	static T* GetComponent( GameObjectID a_GameObjectID )
	{
		T* Found = s_Registry.try_get< T >( a_GameObjectID );

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
			s_TypeMap.InvokeFunction< T* >( HashCode, "GetComponent"_H, Component, a_GameObjectID );

			if ( Component )
			{
				return Component;
			}
		}

		return nullptr;
	}

	template < typename T >
	static bool DestroyComponent( GameObjectID a_GameObjectID )
	{
		return false;//return s_Registry.remove_if_exists< T >( a_GameObjectID );
	}

private:

	template < typename T >
	static T* GetComponentImpl( GameObjectID a_GameObjectID )
	{
		return s_Registry.try_get< T >( a_GameObjectID );
	}

	static TypeMap										s_TypeMap;
	static std::map< size_t, void*(*)( GameObjectID ) > s_TypeAccessors;
	static entt::registry								s_Registry;
};