#pragma once
#include "Component.hpp"
#include "Math.hpp"

template < typename T > class ITransform;
typedef ITransform< void > Transform;

template < typename T >
class ITransform : public IComponent< ITransform< T > >
{
public:

	ITransform()
		: m_GlobalMatrix( Matrix4::Identity )
		, m_LocalMatrix( Matrix4::Identity )
		, m_LocalPosition( Vector3::Zero )
		, m_LocalRotation( Vector3::Zero )
		, m_LocalScale( Vector3::One )
		, m_IsDirty( false )
		, m_Parent( nullptr )
	{ }

	inline Transform* GetParent()
	{
		return m_Parent;
	}

	inline const Transform* GetParent() const
	{
		return m_Parent;
	}

	void SetParent( Transform* a_Parent, bool a_RetainGlobalTransform = true )
	{
		SetParentImpl( a_Parent, a_RetainGlobalTransform );
	}

	inline size_t GetChildCount() const
	{
		return m_Children.size();
	}

	inline Transform* GetChild( size_t a_Index )
	{
		return m_Children[ a_Index ];
	}

	inline void AttachChild( Transform* a_Transform, bool a_RetainGlobalTransform = true )
	{
		if ( !a_Transform )
		{
			return;
		}

		a_Transform->SetParent( this, a_RetainGlobalTransform );
	}

	inline void DetachChild( size_t a_Index, bool a_RetainGlobalTransform = true )
	{
		m_Children[ a_Index ]->SetParentImpl( nullptr, a_RetainGlobalTransform, a_Index );
	}

	inline void DetachChildren( bool a_RetainGlobalTransforms = true )
	{
		for ( size_t i = m_Children.size() - 1; i >= 0; --i )
		{
			DetachChild( i, a_RetainGlobalTransforms );
		}
	}

	inline Vector3 GetLocalPosition() const
	{
		return m_LocalPosition;
	}

	inline float GetLocalPositionX() const
	{
		return m_LocalPosition.x;
	}

	inline float GetLocalPositionY() const
	{
		return m_LocalPosition.y;
	}

	inline float GetLocalPositionZ() const
	{
		return m_LocalPosition.x;
	}

	inline Quaternion GetLocalRotation() const
	{
		
	}

	inline Vector3 GetLocalScale() const
	{
		return m_LocalScale;
	}

	inline float GetLocalScaleX() const
	{
		return m_LocalScale.x;
	}

	inline float GetLocalScaleY() const
	{
		return m_LocalScale.y;
	}

	inline float GetLocalScaleZ() const
	{
		return m_LocalScale.z;
	}

	inline void SetLocalPosition( const Vector3& a_Position )
	{
		m_LocalPosition = a_Position;
		m_IsDirty = true;
	}

	inline void SetLocalPositionX( float a_X )
	{
		m_LocalPosition.x = a_X;
		m_IsDirty = true;
	}

	inline void SetLocalPositionY( float a_Y )
	{
		m_LocalPosition.y = a_Y;
		m_IsDirty = true;
	}

	inline void SetLocalPositionZ( float a_Z )
	{
		m_LocalPosition.z = a_Z;
		m_IsDirty = true;
	}

	inline void SetLocalRotation( const Vector3& a_EulerAngles )
	{
		
	}

	inline void SetLocalRotation( const Quaternion& a_Quaternion )
	{

	}

	inline void SetLocalRotationX( float a_X )
	{
		m_LocalRotation.x = a_X;
		m_IsDirty = true;
	}

	inline void SetLocalRotationY( float a_Y )
	{
		m_LocalRotation.y = a_Y;
		m_IsDirty = true;
	}

	inline void SetLocalRotationZ( float a_Z )
	{
		m_LocalRotation.z = a_Z;
		m_IsDirty = true;
	}

	inline void SetLocalScale( const Vector3& a_Scale )
	{
		m_LocalScale = a_Scale;
		m_IsDirty = true;
	}

	inline void SetLocalScaleX( float a_X )
	{
		m_LocalScale.x = a_X;
		m_IsDirty = true;
	}

	inline void SetLocalScaleY( float a_Y )
	{
		m_LocalScale.y = a_Y;
		m_IsDirty = true;
	}

	inline void SetLocalScaleZ( float a_Z )
	{
		m_LocalScale.z = a_Z;
		m_IsDirty = true;
	}

	inline Vector3 GetGlobalPosition()
	{

	}

	inline float GetGlobalPositionX()
	{

	}

	inline float GetGlobalPositionY()
	{

	}

	inline float GetGlobalPositionZ()
	{

	}

	inline Quaternion GetGlobalRotation()
	{

	}

	inline Vector3 GetGlobalScale()
	{

	}

	inline float GetGlobalScaleX()
	{

	}

	inline float GetGlobalScaleY()
	{

	}

	inline float GetGlobalScaleZ()
	{

	}

	inline void SetGlobalPosition( const Vector3& a_Position )
	{

	}

	inline void SetGlobalPositionX( float a_X )
	{

	}

	inline void SetGlobalPositionY( float a_Y )
	{

	}

	inline void SetGlobalPositionZ( float a_Z )
	{

	}

	inline void SetGlobalRotation( const Vector3& a_EulerAngles )
	{

	}

	inline void SetGlobalRotation( const Quaternion& a_Quaternion )
	{

	}

	inline void SetGlobalScale( const Vector3& a_Scale )
	{

	}

	inline void SetGlobalScaleX( float a_X )
	{

	}

	inline void SetGlobalScaleY( float a_Y )
	{

	}

	inline void SetGlobalScaleZ( float a_Z )
	{

	}

	inline Vector3 GetGlobalForward() const
	{
		return m_GlobalMatrix.c2.ToVector3();
	}

	inline Vector3 GetGlobalBackward() const
	{
		return -m_GlobalMatrix.c2.ToVector3();
	}

	inline Vector3 GetGlobalRight() const
	{
		return m_GlobalMatrix.c0.ToVector3();
	}

	inline Vector3 GetGlobalLeft() const
	{
		return -m_GlobalMatrix.c0.ToVector3();
	}

	inline Vector3 GetGlobalUp() const
	{
		return m_GlobalMatrix.c1.ToVector3();
	}

	inline Vector3 GetGlobalDown() const
	{
		return -m_GlobalMatrix.c1.ToVector3();
	}

	/*inline Vector3 GetGlobalForward() const
	{
		return m_LocalMatrix.c2.ToVector3();
	}

	inline Vector3 GetGlobalBackward() const
	{
		return -m_LocalMatrix.c2.ToVector3();
	}

	inline Vector3 GetGlobalRight() const
	{
		return m_LocalMatrix.c0.ToVector3();
	}

	inline Vector3 GetGlobalLeft() const
	{
		return -m_LocalMatrix.c0.ToVector3();
	}

	inline Vector3 GetGlobalUp() const
	{
		return m_LocalMatrix.c1.ToVector3();
	}

	inline Vector3 GetGlobalDown() const
	{
		return -m_LocalMatrix.c1.ToVector3();
	}*/

	void SetGlobalForward( const Vector3& a_Forward )
	{

	}

	void SetGlobalBackward( const Vector3& a_Backward )
	{

	}

	void SetGlobalRight( const Vector3& a_Right )
	{

	}

	void SetGlobalLeft( const Vector3& a_Left )
	{

	}

	void SetGlobalUp( const Vector3& a_Up )
	{

	}

	void SetGlobalDown( const Vector3& a_Down )
	{

	}

	void SetLocalForward( const Vector3& a_Forward )
	{

	}

	void SetLocalBackward( const Vector3& a_Backward )
	{

	}

	void SetLocalRight( const Vector3& a_Right )
	{

	}

	void SetLocalLeft( const Vector3& a_Left )
	{

	}

	void SetLocalUp( const Vector3& a_Up )
	{

	}

	void SetLocalDown( const Vector3& a_Down )
	{

	}

	inline const Matrix4& GetGlobalMatrix() const
	{
		return m_Parent ? m_GlobalMatrix : m_LocalMatrix;
	}

	inline const Matrix4& GetLocalMatrix() const
	{
		return m_LocalMatrix;
	}

private:

	inline void SetParentImpl( Transform* a_Transform, bool a_RetainGlobalTransform, size_t a_ChildIndex = -1 )
	{
		// unset parent
		if ( m_Parent )
		{
			m_LocalMatrix = m_GlobalMatrix;
			auto Where = a_ChildIndex == -1 ? std::find( m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this ) : m_Parent->m_Children.begin() + a_ChildIndex;
			m_Parent->m_Children.erase( Where );
		}

		// set parent
		if ( a_Transform )
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( a_Transform->m_GlobalMatrix ), m_LocalMatrix );
			a_Transform->m_Children.push_back( this );
		}

		m_Parent = a_Transform;
	}

	void UpdateTransform()
	{
		if ( m_IsDirty )
		{
			m_LocalMatrix = Matrix4::CreateTransform( m_LocalPosition, m_LocalRotation, m_LocalScale );
			m_IsDirty = false;
		}

		if ( m_Parent )
		{
			m_LocalMatrix = Math::Multiply( m_Parent->GetGlobalMatrix(), m_LocalMatrix );
		}

		for ( auto Begin = m_Children.begin(), End = m_Children.end(); Begin != End; ++Begin )
		{
			( *Begin )->UpdateTransform();
		}
	}

	Matrix4    m_GlobalMatrix;
	Matrix4    m_LocalMatrix;
	Vector3    m_LocalPosition;
	Vector3    m_LocalRotation;
	Vector3    m_LocalScale;
	bool       m_IsDirty;
	Transform* m_Parent;
	std::vector< Transform* > m_Children;
};