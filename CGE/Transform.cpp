#include "Transform.hpp"
#include "ECS.hpp"

Transform* GetTransform( GameObjectID a_GameObjectID )
{
	return ECS::GetComponent< Transform >( a_GameObjectID );
}