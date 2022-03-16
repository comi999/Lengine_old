#include "ECS.hpp"

TypeMap ECS::s_TypeMap;
std::map< size_t, void*(*)( GameObjectID ) > ECS::s_TypeAccessors;
entt::registry ECS::s_Registry;