#pragma once
#include "Component.hpp"
#include "Transform.hpp"
#include "Alias.hpp"
#include "Name.hpp"

typedef uint32_t GameObjectID;

class GameObject
{
public:

	inline static constexpr GameObjectID Null = -1;

	constexpr GameObject()
		: m_ID( -1 )
	{ }

	constexpr GameObject( GameObjectID a_ID )
		: m_ID( a_ID )
	{ }

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

	static GameObject Instantiate( GameObject a_Parent )
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
	}

	inline static GameObject FindByID( GameObjectID a_GameObjectID )
	{
		return *reinterpret_cast< GameObject* >( &a_GameObjectID );
	}

	inline static GameObject FindByName( Hash a_Name )
	{
		return GameObject::FindByID( Alias::FindByName( a_Name ) );
	}

	inline static void Destroy( GameObject a_GameObject )
	{
		Component::Destroy< Transform >( a_GameObject );
	}

	inline bool IsValid() const
	{
		return m_ID != Null && ComponentBase::GetRegistry().valid( entt::entity( m_ID ) );
	}

	template < typename T >
	inline T* AddComponent()
	{
		return Component::AddComponent< T >( m_ID );
	}

	template < typename T >
	inline T* GetComponent( bool a_ExactOnly = false )
	{
		return a_ExactOnly ? Component::GetExactComponent< T >( m_ID ) : Component::GetComponent< T >( m_ID );
	}

	template < typename T >
	inline const T* GetComponent( bool a_ExactOnly = false ) const
	{
		return const_cast< GameObject* >( this )->GetComponent< T >( a_ExactOnly );
	}

	template < typename T >
	inline std::list< T* > GetComponents( bool a_ExactOnly = false )
	{
		return a_ExactOnly ? Component::GetExactComponents< T >( m_ID ) : Component::GetComponents< T >( m_ID );
	}

	template < typename T >
	inline std::list< const T* > GetComponents( bool a_ExactOnly = false ) const
	{
		return a_ExactOnly ? Component::GetExactComponents< T >( m_ID ) : Component::GetComponents< T >( m_ID );
	}

	template < typename T >
	T* GetComponentInChild( bool a_ExactOnly = false )
	{
		auto& ThisTransform = *GetTransform();

		for ( auto& ChildTransform : ThisTransform )
		{
			if ( T* ChildComponent = ChildTransform.GetOwner()->GetComponent< T >( a_ExactOnly ) )
			{
				return ChildComponent;
			}
		}

		for ( auto& ChildTransform : ThisTransform )
		{
			if ( T* ChildComponent = ChildTransform.GetOwner()->GetComponentInChild< T >( a_ExactOnly ) )
			{
				return ChildComponent;
			}
		}

		return nullptr;
	}

	template < typename T >
	const T* GetComponentInChild( bool a_ExactOnly = false ) const
	{
		return const_cast< GameObject* >( this )->GetComponentInChild< T >( a_ExactOnly );
	}

	template < typename T >
	std::list< T* > GetComponentsInChildren( bool a_ExactOnly = false )
	{
		return std::list< T* >();
	}

	template < typename T >
	std::list< const T* > GetComponentsInChildren( bool a_ExactOnly ) const
	{
		return std::list< const T* >();
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

	inline constexpr GameObjectID GetID() const
	{
		return m_ID;
	}

	constexpr operator GameObjectID() const
	{
		return m_ID;
	}

	inline bool operator==( GameObjectID a_ID ) const
	{
		return m_ID == a_ID;
	}

	inline bool operator!=( GameObjectID a_ID ) const
	{
		return m_ID != a_ID;
	}

private:

	friend class ComponentBase;

	GameObjectID m_ID;
};