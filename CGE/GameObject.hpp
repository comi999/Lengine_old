#pragma once
#include "Object.hpp"
#include "SceneHeirarchy.hpp"
#include "Transform.hpp"

//typedef ObjectID GameObjectID;
//
//class GameObject : public Object
//{
//private:
//
//	GameObject( const Name& a_Name, GameObjectID a_ObjectID, SceneID a_SceneID )
//		: Object( a_Name, a_ObjectID )
//		, m_SceneID( a_SceneID )
//	{ }
//
//public:
//
//	Transform& GetTransform()
//	{
//		
//	}
//
//	const Transform& GetTransform() const
//	{
//
//	}
//
//	template < typename _Component >
//	_Component& AddComponent()
//	{
//
//	}
//
//	template < typename _Component >
//	_Component* GetComponent()
//	{
//
//	}
//
//	template < typename _Component >
//	std::list< _Component* > GetComponents()
//	{
//
//	}
//
//	template < typename _Component >
//	_Component* GetComponentInChild()
//	{
//
//	}
//
//	template < typename _Component >
//	std::list< _Component* > GetComponentsInChildren()
//	{
//
//	}
//
//	template < typename _Component >
//	bool DestroyComponent()
//	{
//
//	}
//
//	static GameObject& Instantiate()
//	{
//
//	}
//
//	static GameObject& Instantiate( const Name& a_Name )
//	{
//
//	}
//
//private:
//
//	SceneID                               m_SceneID;
//	static std::map< Hash, GameObjectID > s_GameObjectLookup;
//	static std::vector< GameObject >      s_GameObjects;
//};