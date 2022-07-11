#pragma once
#include "Component.hpp"
#include "Math.hpp"

DefineComponent( Light, Component )
{
public:

	ILight()
		: m_Direction( 0.0f, -1.0f, 0.0f )
	{ }

	void SetDirection( const Vector3& a_Direction )
	{
		m_Direction = a_Direction;
	}

	const Vector3& GetDirection() const
	{
		return m_Direction;
	}

	static void SetSun( const Light* a_Light )
	{
		s_Sun = a_Light ? a_Light->GetOwner().GetID() : GameObjectID( -1 );
	}

	static const Light* GetSun()
	{
		return s_Sun != GameObjectID( -1 ) ? GameObject::FindByID( s_Sun ).GetComponent< Light >() : nullptr;
	}

private:

	Vector3 m_Direction;
	inline static GameObjectID s_Sun = GameObjectID( -1 );
};