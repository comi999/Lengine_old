#pragma once
#include "Object.hpp"
#include "GameObject.hpp"
#include "entt/entt.hpp"

typedef ObjectID ComponentID;

class Component : public Object
{
protected:

	Component( const Name& a_Name, ObjectID a_ObjectID, GameObject& a_GameObject )
		: Object( a_Name, a_ObjectID )
		, m_GameObject( &a_GameObject )
	{ }

	inline ComponentID GetComponentID() const
	{
		return Object::GetObjectID();
	}

	inline GameObject& GetGameObject()
	{
		//return *m_GameObject;
	}

	inline const GameObject& GetGameObject() const
	{
		//return *m_GameObject;
	}

	inline GameObjectID GetGameObjectID() const
	{
		return m_GameObjectID;
	}

	inline Transform& GetTransform()
	{
		//return m_GameObject->GetTransform();
	}

	inline const Transform& GetTransform() const
	{
		//return m_GameObject->GetTransform();
	}

private:

	GameObjectID m_GameObjectID;
	static entt::basic_registry< GameObject > s_Registry;
};