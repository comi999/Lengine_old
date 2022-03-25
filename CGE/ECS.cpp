#include "ECS.hpp"
#include "Alias.hpp"
#include "Transform.hpp"

TypeMap        ECS::s_TypeMap;
entt::registry ECS::s_Registry;

void ECS::AttachTransform( GameObjectID a_GameObjectID, GameObjectID a_Parent )
{
	AddComponent< Transform >( a_GameObjectID )->SetParent( a_Parent );
}

void ECS::AttachAlias( GameObjectID a_GameObjectID, const Name& a_Name )
{
	AddComponent< Alias >( a_GameObjectID )->SetName( a_Name );
}