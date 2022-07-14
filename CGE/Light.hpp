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

	void SetAmbient( const Vector3& a_Ambient )
	{
		m_Ambient = a_Ambient;
	}

	const Vector3& GetAmbient() const
	{
		return m_Ambient;
	}

	static void SetSun( const Light* a_Light )
	{
		s_Sun = a_Light ? a_Light->GetOwner().GetID() : GameObject::Null;
	}

	static Light* GetSun()
	{
		GameObject SunObject = GameObject::FindByID( s_Sun );

		if ( !SunObject.IsValid() )
		{
			s_Sun = GameObject::Null;
			return nullptr;
		}

		return SunObject.GetComponent< Light >();
	}

private:

	Vector3 m_Direction;
	Vector3 m_Ambient;
	inline static GameObjectID s_Sun = GameObjectID( -1 );
};