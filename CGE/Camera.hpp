#pragma once
#include "Component.hpp"
#include "Math.hpp"
#include "ScreenBuffer.hpp"
#include "GameObject.hpp"
#include "Transform.hpp"

DefineComponent( Camera, Component )
{
public:

	inline Matrix4 GetProjectionMatrix() const
	{
		if ( m_Dirty )
		{
			const_cast< Camera* >( this )->m_Projection = Matrix4::CreateProjection( m_FOV, m_Aspect, m_NearZ, m_FarZ );
			const_cast< Camera* >( this )->m_Dirty = false;
		}

		return m_Projection;
	}

	inline Matrix4 GetViewMatrix() const
	{
		const Transform* OwnerTransform = this->GetOwner().GetTransform();
		return Matrix4::CreateView( OwnerTransform->GetGlobalPosition(), OwnerTransform->GetGlobalRotation() );
	}

	inline Matrix4 GetProjectionViewMatrix() const
	{
		return Math::Multiply( GetProjectionMatrix(), GetViewMatrix() );
	}

	inline float GetFOV() const
	{
		return m_FOV;
	}

	inline void SetFOV( float a_Radians )
	{
		m_FOV = a_Radians;
		m_Dirty = true;
	}

	inline float GetAspect() const
	{
		return m_Aspect;
	}

	inline void SetAspect( float a_Aspect )
	{
		m_Aspect = a_Aspect;
		m_Dirty = true;
	}

	inline float GetNearZ() const
	{
		return m_FarZ;
	}

	inline void SetNearZ( float a_NearZ )
	{
		m_NearZ = a_NearZ;
		m_Dirty = true;
	}

	inline float GetFarZ() const
	{
		return m_FarZ;
	}

	inline void SetFarZ( float a_FarZ )
	{
		m_FarZ = a_FarZ;
		m_Dirty;
	}

	inline static void SetMainCamera( const Camera* a_Camera )
	{
		s_MainCamera = a_Camera ? a_Camera->GetOwner() : GameObjectID( -1 );
	}

	inline static const Camera* GetMainCamera()
	{
		return s_MainCamera != GameObjectID( -1 ) ? GameObject::FindByID( s_MainCamera ).GetComponent< Camera >() : nullptr;
	}

private:

	Matrix4 m_Projection;
	float   m_FOV;
	float   m_Aspect;
	float   m_NearZ;
	float   m_FarZ;
	bool    m_Dirty;

	inline static GameObjectID s_MainCamera = GameObjectID( -1 );
};