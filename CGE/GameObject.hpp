#pragma once
#include "Component.hpp"
#include "Transform.hpp"
#include "Alias.hpp"
#include "Name.hpp"

typedef uint32_t GameObjectID;

class GameObject
{
public:

	static GameObject Instantiate()
	{
		entt::entity NewEntity = ComponentBase::GetRegistry().create();
		GameObject NewGameObject = reinterpret_cast< GameObject& >( NewEntity );
		NewGameObject.AddComponent< Alias >();
		NewGameObject.AddComponent< Transform >();
		return NewGameObject;
	}

	static GameObject Instantiate( const Name& a_Name )
	{
		entt::entity NewEntity = ComponentBase::GetRegistry().create();
		GameObject NewGameObject = reinterpret_cast< GameObject& >( NewEntity );
		NewGameObject.AddComponent< Alias >()->SetName( a_Name );
		NewGameObject.AddComponent< Transform >();
		return NewGameObject;
	}

	/*static GameObject Instantiate( GameObject a_Parent )
	{
		entt::entity NewEntity = ComponentBase::GetRegistry().create();
		GameObject NewGameObject = reinterpret_cast< GameObject& >( NewEntity );
		NewGameObject.AddComponent< Alias >();
		NewGameObject.AddComponent< Transform >()->SetParent( a_Parent );
		return NewGameObject;
	}

	static GameObject Instantiate( const Name& a_Name, GameObject a_Parent )
	{
		entt::entity NewEntity = ComponentBase::GetRegistry().create();
		GameObject NewGameObject = reinterpret_cast< GameObject& >( NewEntity );
		NewGameObject.AddComponent< Alias >()->SetName( a_Name );
		NewGameObject.AddComponent< Transform >()->SetParent( a_Parent );
		return NewGameObject;
	}*/

	inline static GameObject FindByID( GameObjectID a_GameObjectID )
	{
		return *reinterpret_cast< GameObject* >( &a_GameObjectID );
	}

	inline static GameObject FindByName( Hash a_Name )
	{
		return GameObject::FindByID( Alias::FindByName( a_Name ) );
	}

	inline static void Destroy( GameObjectID a_GameObjectID )
	{
		// Implementation
	}

	inline bool IsValid() const
	{
		return m_ID == static_cast< GameObjectID >( -1 );
	}

	template < typename T >
	inline T* AddComponent()
	{
		return Component::AddComponent< T >( m_ID );
	}

	template < typename T >
	inline T* GetComponent()
	{
		return Component::GetComponent< T >( m_ID );
	}

	template < typename T >
	inline const T* GetComponent() const
	{
		return Component::GetComponent< T >( m_ID );
	}

	template < typename T >
	inline T* GetExactComponent()
	{
		return Component::GetExactComponent< T >( m_ID );
	}

	template < typename T >
	inline const T* GetExactComponent() const
	{
		return Component::GetExactComponent< T >( m_ID );
	}

	template < typename T >
	inline bool DestroyComponent()
	{
		return Component::DestroyComponent< T >( m_ID );
	}

	inline Transform* GetTransform()
	{
		return Component::GetExactComponent< Transform >( m_ID );
	}

	inline const Transform* GetTransform() const
	{
		return Component::GetExactComponent< Transform >( m_ID );
	}

	inline Alias* GetAlias()
	{
		return Component::GetExactComponent< Alias >( m_ID );
	}

	inline const Alias* GetAlias() const
	{
		return Component::GetExactComponent< Alias >( m_ID );
	}

	inline GameObjectID GetID() const
	{
		return m_ID;
	}

	operator GameObjectID() const
	{
		return m_ID;
	}

private:

	friend class ComponentBase;

	GameObjectID m_ID;
};