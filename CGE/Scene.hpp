//#pragma once
//#include "Object.hpp"
//#include "SceneHeirarchy.hpp"
//#include "ComponentRegistry.hpp"
//
//typedef ObjectID SceneID;
//
//class Scene : protected Object
//{
//	Scene( const Name& a_Name, SceneID a_SceneID )
//		: Object( a_Name, a_SceneID )
//		, m_SceneHeirarchy( *this )
//	{ }
//
//public:
//
//	inline const Name& GetName() const
//	{
//		return Object::GetName();
//	}
//
//	inline void SetName( const Name& a_Name )
//	{
//		Object::SetName( a_Name );
//	}
//
//	inline SceneID GetSceneID() const
//	{
//		return Object::GetObjectID();
//	}
//
//private:
//
//	SceneHeirarchy                     m_SceneHeirarchy;
//	static ComponentRegistry           s_Components;
//	static std::list< Scene >          s_Scenes;
//	static std::vector< GameObjectID > s_GameObjects;
//};