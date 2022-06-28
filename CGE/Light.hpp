#pragma once
#include "Component.hpp"
#include "Math.hpp"

DefineComponent( Light, Component )
{
public:

	void SetDirection( const Vector3& a_Direction )
	{
		m_Direction = a_Direction;
	}

	const Vector3& GetDirection()
	{
		return m_Direction;
	}

private:

	Vector3 m_Direction;
};