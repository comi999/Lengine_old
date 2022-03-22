#pragma once
#include "Component.hpp"
#include "Math.hpp"
#include "ScreenBuffer.hpp"
#include "GameObject.hpp"
#include "Transform.hpp"

struct Frustum
{

};

template < typename T >
class ICamera;

typedef ICamera< void > Camera;

template < typename T >
class ICamera : public IComponent< ICamera< T > >
{
public:

	inline Matrix4 GetProjectionMatrix()
	{
		if ( m_Dirty )
		{
			float AspectRatio = static_cast< float >( ScreenBuffer::GetBufferWidth() ) / ScreenBuffer::GetBufferHeight();
			m_Projection = Matrix4::CreateProjection( m_FOV, AspectRatio, m_NearZ, m_FarZ );
			m_Dirty = false;
		}

		return m_Projection;
	}

	inline Matrix4 GetViewMatrix()
	{
		Transform& OwnerTransform = ComponentBase::GetGameObject().GetTransform();
		return Matrix4::CreateView( OwnerTransform.GetGlobalPosition(), OwnerTransform.GetGlobalRotation() );
	}

	inline Matrix4 GetProjectionViewMatrix()
	{
		return Math::Multiply( GetProjectionMatrix(), GetViewMatrix() );
	}

	inline float GetFOV() const
	{
		return m_FOV;
	}

	inline void SetFOV( float a_Degrees )
	{
		m_FOV = Math::Radians( a_Degrees );
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

private:

	Matrix4 m_Projection;
	float   m_FOV;
	float   m_NearZ;
	float   m_FarZ;
	bool    m_Dirty;
};