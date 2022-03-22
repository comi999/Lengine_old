#pragma once
//#include "Object.hpp"
#include "Transform.hpp"
#include "ECS.hpp"
#include "Name.hpp"

typedef entt::entity GameObjectID;

class GameObject : public IComponent< GameObject >
{
private:

public:

	template < typename T >
	T* AddComponent()
	{
		return ECS::AddComponent< T >( m_ID );
	}

	template < typename T >
	T* GetComponent()
	{
		return ECS::GetComponent< T >( ComponentBase:: );
	}

	Transform* GetTransform()
	{
		return ECS::GetComponent< Transform >( m_ID );
	}

private:

	Name m_Name;

	/*GameObject( const Name& a_Name, GameObjectID a_ObjectID )
		: Object( a_Name, ObjectID( a_ObjectID ) )
		, m_ToDestroy( false )
	{
		ECS::AddComponent< Transform >( *this, GameObjectID( a_ObjectID ) );
	}

public:

	inline Transform& GetTransform()
	{
		return *ECS::GetComponent< Transform >( GameObjectID( GetObjectID() ) );
	}

	const Transform& GetTransform() const
	{
		return *ECS::GetComponent< Transform >( GameObjectID( GetObjectID() ) );
	}

	template < typename T >
	T* AddComponent()
	{
		return ECS::AddComponent< T >( *this, GameObjectID( GetObjectID() ) );
	}

	template < typename T >
	T* GetComponent()
	{
		return ECS::GetComponent< T >( GameObjectID( GetObjectID() ) );
	}

	template < typename _Component >
	std::list< _Component* > GetComponents()
	{
		return std::list< _Component* >();
	}

	template < typename _Component >
	_Component* GetComponentInChild()
	{
		return nullptr;
	}

	template < typename _Component >
	std::list< _Component* > GetComponentsInChildren()
	{
		return std::list< _Component* >();
	}

	template < typename _Component >
	bool DestroyComponent()
	{
		return false;
	}

	static GameObject& Instantiate()
	{
		static size_t Instantiated = 0;
		return Instantiate( Name( "NewGameObject" + std::to_string( Instantiated++ ) ) );
	}

	static GameObject& Instantiate( const Name& a_Name )
	{
		auto Found = s_GameObjectLookup.find( a_Name.HashCode() );
		
		if ( Found != s_GameObjectLookup.end() )
		{
			Found->second.push_back( GameObject( a_Name, ECS::GenerateGameObjectID() ) );
			return Found->second.back();
		}
		
		auto& NewVector = s_GameObjectLookup.emplace( a_Name, std::vector< GameObject >() ).first->second;
		NewVector.push_back( GameObject( a_Name, ECS::GenerateGameObjectID() ) );
		return NewVector.back();
	}

	static GameObject* Find( const Name& a_Name )
	{
		if ( auto Found = s_GameObjectLookup.find( a_Name.HashCode() ); Found != s_GameObjectLookup.end() )
		{
			return &Found->second.front();
		}

		return nullptr;
	}

private:

	template < class, class > class std::vector;

	bool m_ToDestroy;
	
	static std::map< Hash, std::vector< GameObject > > s_GameObjectLookup;*/
};