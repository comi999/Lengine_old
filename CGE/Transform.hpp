#pragma once
#include "Component.hpp"
#include "Math.hpp"

typedef uint32_t GameObjectID;

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
		, m_LocalScale( Vector3::One )
		, m_IsDirty( true )
		, m_Parent( GameObjectID( -1 ) )
	{ }

	inline void SetDirty()
	{
		m_IsDirty = true;
	}

	inline Transform* GetParent()
	{
		return m_Parent == GameObjectID( -1 ) ? nullptr : reinterpret_cast< GameObject* >( &m_Parent )->GetTransform();
	}

	inline const Transform* GetParent() const
	{
		return m_Parent == GameObjectID( -1 ) ? nullptr : reinterpret_cast< GameObject* >( &m_Parent )->GetTransform();
	}

	inline void SetParent( Transform* a_Parent, bool a_RetainGlobalTransform = true )
	{
		SetParentImpl( a_Parent, a_RetainGlobalTransform );
	}

	inline void SetParent( GameObjectID a_Parent, bool a_RetainGlobalTransform = true )
	{
		SetParentImpl( ECS::GetComponent< Transform >( a_Parent ), a_RetainGlobalTransform );
	}

	inline void DetachFromParent( bool a_RetainGlobalTransform = true )
	{
		SetParentImpl( nullptr, a_RetainGlobalTransform );
	}

	inline size_t GetChildCount() const
	{
		return m_Children.size();
	}

	inline Transform* GetChild( size_t a_Index )
	{
		return GameObject::FindByID( m_Children[ a_Index ] ).GetTransform();
	}

	inline void AttachChild( Transform* a_Transform, bool a_RetainGlobalTransform = true )
	{
		if ( !a_Transform )
		{
			return;
		}

		a_Transform->SetParent( this, a_RetainGlobalTransform );
	}

	inline void AttachChild( GameObjectID a_Transform, bool a_RetainGlobalTransform = true )
	{
		//implementation
	}

	inline void DetachChild( size_t a_Index, bool a_RetainGlobalTransform = true )
	{
		GameObject::FindByID( m_Children[ a_Index ] ).GetTransform()->SetParentImpl( nullptr, a_RetainGlobalTransform, a_Index );
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
		return m_LocalRotation;
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

	inline void SetLocalRotation( const Quaternion& a_Rotation )
	{
		m_LocalRotation = a_Rotation;
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

	inline Vector3 GetGlobalPosition() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractTranslation( m_GlobalMatrix ) : m_LocalPosition;
	}

	inline float GetGlobalPositionX() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractTranslationX( m_GlobalMatrix ) : m_LocalPosition.x;
	}

	inline float GetGlobalPositionY() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractTranslationY( m_GlobalMatrix ) : m_LocalPosition.y;
	}

	inline float GetGlobalPositionZ() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractTranslationZ( m_GlobalMatrix ) : m_LocalPosition.z;
	}

	inline Quaternion GetGlobalRotation() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractRotation( m_GlobalMatrix ) : m_LocalRotation;
	}

	inline Vector3 GetGlobalScale() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractScale( m_GlobalMatrix ) : m_LocalScale;
	}

	inline float GetGlobalScaleX() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractScaleX( m_GlobalMatrix ) : m_LocalScale.x;
	}

	inline float GetGlobalScaleY() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractScaleY( m_GlobalMatrix ) : m_LocalScale.y;
	}

	inline float GetGlobalScaleZ() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? Matrix4::ExtractScaleZ( m_GlobalMatrix ) : m_LocalScale.z;
	}

	inline void SetGlobalPosition( const Vector3& a_Position )
	{
		Matrix4::SetTranslation( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_Position );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalPosition = a_Position;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalPosition = Matrix4::ExtractTranslation( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline void SetGlobalPositionX( float a_X )
	{
		Matrix4::SetTranslationX( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_X );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalPosition.x = a_X;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalPosition = Matrix4::ExtractTranslation( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline void SetGlobalPositionY( float a_Y )
	{
		Matrix4::SetTranslationY( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_Y );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalPosition.y = a_Y;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalPosition = Matrix4::ExtractTranslation( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline void SetGlobalPositionZ( float a_Z )
	{
		Matrix4::SetTranslationZ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_Z );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalPosition.z = a_Z;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalPosition = Matrix4::ExtractTranslation( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline void SetGlobalRotation( const Quaternion& a_Rotation )
	{
		Matrix4::SetRotation( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_Rotation );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalRotation = a_Rotation;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalRotation = Matrix4::ExtractRotation( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline void SetGlobalScale( const Vector3& a_Scale )
	{
		Matrix4::SetScale( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_Scale );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalScale = a_Scale;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalScale = Matrix4::ExtractScale( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline void SetGlobalScaleX( float a_X )
	{
		Matrix4::SetScaleX( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_X );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalScale.x = a_X;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalScale = Matrix4::ExtractScale( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline void SetGlobalScaleY( float a_Y )
	{
		Matrix4::SetScaleY( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_Y );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalScale.y = a_Y;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalScale = Matrix4::ExtractScale( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline void SetGlobalScaleZ( float a_Z )
	{
		Matrix4::SetScaleZ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix, a_Z );
		
		if ( m_Parent == static_cast< GameObjectID >( -1 ) )
		{
			m_LocalScale.z = a_Z;
		}
		else
		{
			m_LocalMatrix = Math::Multiply( Math::Inverse( GameObject::FindByID( m_Parent ).GetTransform()->m_GlobalMatrix ), m_GlobalMatrix );
			m_LocalScale = Matrix4::ExtractScale( m_LocalMatrix );
		}

		for ( auto Child : m_Children )
		{
			GameObject::FindByID( Child ).GetTransform()->m_IsDirty = true;
		}
	}

	inline Vector3 GetLocalForward() const
	{
		return Math::Normalize( m_LocalMatrix.c2.ToVector3() );
	}

	inline Vector3 GetLocalBackward() const
	{
		return -Math::Normalize( m_LocalMatrix.c2.ToVector3() );
	}

	inline Vector3 GetLocalRight() const
	{
		return Math::Normalize( m_LocalMatrix.c0.ToVector3() );
	}

	inline Vector3 GetLocalLeft() const
	{
		return -Math::Normalize( m_LocalMatrix.c0.ToVector3() );
	}

	inline Vector3 GetLocalUp() const
	{
		return Math::Normalize( m_LocalMatrix.c1.ToVector3() );
	}

	inline Vector3 GetLocalDown() const
	{
		return -Math::Normalize( m_LocalMatrix.c1.ToVector3() );
	}

	inline Vector3 GetGlobalForward() const
	{
		return Math::Normalize( ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ).c2.ToVector3() );
	}

	inline Vector3 GetGlobalBackward() const
	{
		return -Math::Normalize( ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ).c2.ToVector3() );
	}

	inline Vector3 GetGlobalRight() const
	{
		return Math::Normalize( ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ).c0.ToVector3() );
	}

	inline Vector3 GetGlobalLeft() const
	{
		return -Math::Normalize( ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ).c0.ToVector3() );
	}

	inline Vector3 GetGlobalUp() const
	{
		return Math::Normalize( ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ).c1.ToVector3() );
	}

	inline Vector3 GetGlobalDown() const
	{
		return -Math::Normalize( ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ).c1.ToVector3() );
	}

	void SetLocalForward( const Vector3& a_Forward )
	{
		Matrix3 Rotation;
		Rotation.c0 = Math::Normalize( Math::Cross( Vector3::Up, a_Forward ) );
		Rotation.c1 = Math::Normalize( Math::Cross( a_Forward, Rotation.c0.ToVector() ) );
		Rotation.c2 = a_Forward;
		m_LocalRotation = Quaternion::ToQuaternion( Rotation );
		m_IsDirty = true;
	}

	void SetLocalBackward( const Vector3& a_Backward )
	{
		Matrix3 Rotation;
		Rotation.c0 = Math::Normalize( Math::Cross( a_Backward, Vector3::Up ) );
		Rotation.c1 = Math::Normalize( Math::Cross( Rotation.c0.ToVector(), a_Backward ) );
		Rotation.c2 = -a_Backward;
		m_LocalRotation = Quaternion::ToQuaternion( Rotation );
		m_IsDirty = true;
	}

	void SetLocalRight( const Vector3& a_Right )
	{
		Matrix3 Rotation;
		Rotation.c2 = Math::Normalize( Math::Cross( a_Right, Vector3::Up ) );
		Rotation.c1 = Math::Normalize( Math::Cross( Rotation.c2.ToVector(), a_Right ) );
		Rotation.c0 = a_Right;
		m_LocalRotation = Quaternion::ToQuaternion( Rotation );
		m_IsDirty = true;
	}

	void SetLocalLeft( const Vector3& a_Left )
	{
		Matrix3 Rotation;
		Rotation.c2 = Math::Normalize( Math::Cross( Vector3::Up, a_Left ) );
		Rotation.c1 = Math::Normalize( Math::Cross( a_Left, Rotation.c2.ToVector() ) );
		Rotation.c0 = -a_Left;
		m_LocalRotation = Quaternion::ToQuaternion( Rotation );
		m_IsDirty = true;
	}

	void SetGlobalForward( const Vector3& a_Forward )
	{
		Matrix3 Rotation;
		Rotation.c0 = Math::Normalize( Math::Cross( Vector3::Up, a_Forward ) );
		Rotation.c1 = Math::Normalize( Math::Cross( a_Forward, Rotation.c0.ToVector() ) );
		Rotation.c2 = a_Forward;
		SetGlobalRotation( Quaternion::ToQuaternion( Rotation ) );
	
	}

	void SetGlobalBackward( const Vector3& a_Backward )
	{
		Matrix3 Rotation;
		Rotation.c0 = Math::Normalize( Math::Cross( a_Backward, Vector3::Up ) );
		Rotation.c1 = Math::Normalize( Math::Cross( Rotation.c0.ToVector(), a_Backward ) );
		Rotation.c2 = -a_Backward;
		SetGlobalRotation( Quaternion::ToQuaternion( Rotation ) );
	}

	void SetGlobalRight( const Vector3& a_Right )
	{
		Matrix3 Rotation;
		Rotation.c2 = Math::Normalize( Math::Cross( a_Right, Vector3::Up ) );
		Rotation.c1 = Math::Normalize( Math::Cross( Rotation.c2.ToVector(), a_Right ) );
		Rotation.c0 = a_Right;
		SetGlobalRotation( Quaternion::ToQuaternion( Rotation ) );
	}

	void SetGlobalLeft( const Vector3& a_Left )
	{
		Matrix3 Rotation;
		Rotation.c2 = Math::Normalize( Math::Cross( Vector3::Up, a_Left ) );
		Rotation.c1 = Math::Normalize( Math::Cross( a_Left, Rotation.c2.ToVector() ) );
		Rotation.c0 = -a_Left;
		SetGlobalRotation( Quaternion::ToQuaternion( Rotation ) );
	}

	void TranslateLocal( const Vector3& a_Translation )
	{
		m_LocalPosition += a_Translation;
		m_IsDirty = true;
	}

	void TranslateLocalX( float a_X )
	{
		m_LocalPosition.x += a_X;
		m_IsDirty = true;
	}

	void TranslateLocalY( float a_Y )
	{
		m_LocalPosition.y = a_Y;
		m_IsDirty = true;
	}

	void TranslateLocalZ( float a_Z )
	{
		m_LocalPosition.z = a_Z;
		m_IsDirty = true;
	}

	inline void TranslateGlobal( const Vector3& a_Translation )
	{
		SetGlobalPosition( a_Translation + Matrix4::ExtractTranslation( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ) );
	}

	inline void TranslateGlobalX( float a_X )
	{
		SetGlobalPositionX( a_X + Matrix4::ExtractTranslationX( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ) );
	}

	inline void TranslateGlobalY( float a_Y )
	{
		SetGlobalPositionY( a_Y + Matrix4::ExtractTranslationY( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ) );
	}

	inline void TranslateGlobalZ( float a_Z )
	{
		SetGlobalPositionZ( a_Z + Matrix4::ExtractTranslationZ( m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix ) );
	}

	void RotateLocal( const Quaternion& a_Rotation )
	{
		m_LocalRotation = Quaternion::Concatenate( a_Rotation, m_LocalRotation );
		m_IsDirty = true;
	}

	inline void RotateGlobal( const Quaternion& a_Rotation )
	{
		SetGlobalRotation( Quaternion::Concatenate( a_Rotation, GetGlobalRotation() ) );
	}

	inline void ScaleLocal( const Vector3& a_Scale )
	{
		SetLocalScale( GetLocalScale() * a_Scale );
	}

	inline void ScaleLocalX( float a_X )
	{
		SetLocalScaleX( GetLocalScaleX() * a_X );
	}

	inline void ScaleLocalY( float a_Y )
	{
		SetLocalScaleY( GetLocalScaleY() * a_Y );
	}

	inline void ScaleLocalZ( float a_Z )
	{
		SetLocalScaleZ( GetLocalScaleZ() * a_Z );
	}

	inline void ScaleGlobal( const Vector3& a_Scale )
	{
		SetGlobalScale( GetGlobalScale() * a_Scale );
	}

	inline void ScaleGlobalX( float a_X )
	{
		SetGlobalScaleX( GetGlobalScaleX() * a_X );
	}

	inline void ScaleGlobalY( float a_Y )
	{
		SetGlobalScaleY( GetGlobalScaleY() * a_Y );
	}

	inline void ScaleGlobalZ( float a_Z )
	{
		SetGlobalScaleZ( GetGlobalScaleZ() * a_Z );
	}

	inline const Matrix4& GetGlobalMatrix() const
	{
		return m_Parent != static_cast< GameObjectID >( -1 ) ? m_GlobalMatrix : m_LocalMatrix;
	}

	inline const Matrix4& GetLocalMatrix() const
	{
		return m_LocalMatrix;
	}

//private:

	inline void SetParentImpl( Transform* a_Transform, bool a_RetainGlobalTransform, size_t a_ChildIndex = -1 )
	{
		GameObjectID ThisID = this->GetOwnerID();

		// unset parent
		if ( m_Parent != static_cast< GameObjectID >( -1 ) )
		{
			Transform* ParentTransform = ECS::GetExactComponent< Transform >( m_Parent );

			if ( a_RetainGlobalTransform )
			{
				m_LocalMatrix = m_GlobalMatrix;
				Matrix4::Decompose( m_LocalMatrix, m_LocalPosition, m_LocalRotation, m_LocalScale );
			}

			auto Where = a_ChildIndex == -1 ? std::find( ParentTransform->m_Children.begin(), ParentTransform->m_Children.end(), ThisID ) : ParentTransform->m_Children.begin() + a_ChildIndex;
			ParentTransform->m_Children.erase( Where );
		}

		// set parent
		if ( a_Transform )
		{
			if ( a_RetainGlobalTransform )
			{ 
				m_LocalMatrix = Math::Multiply( Math::Inverse( a_Transform->GetGlobalMatrix() ), m_LocalMatrix );
				Matrix4::Decompose( m_LocalMatrix, m_LocalPosition, m_LocalRotation, m_LocalScale );
			}

			a_Transform->m_Children.push_back( ThisID );
			m_Parent = a_Transform->GetOwnerID();
		}
		else
		{
			m_Parent = static_cast< GameObjectID >( -1 );
		}
	}

	void UpdateTransform()
	{
		if ( m_IsDirty )
		{
			m_LocalMatrix = Matrix4::CreateTransform( m_LocalPosition, m_LocalRotation, m_LocalScale );
			m_IsDirty = false;
		}

		if ( m_Parent != GameObjectID( -1 ) )
		{
			m_GlobalMatrix = Math::Multiply( GetParent()->GetGlobalMatrix(), m_LocalMatrix );
		}

		for ( auto Begin = m_Children.begin(), End = m_Children.end(); Begin != End; ++Begin )
		{
			GameObject::FindByID( *Begin ).GetTransform()->UpdateTransform();
		}
	}

	Matrix4      m_GlobalMatrix;
	Matrix4      m_LocalMatrix;
	Vector3      m_LocalPosition;
	Quaternion   m_LocalRotation;
	Vector3      m_LocalScale;
	bool         m_IsDirty;
	GameObjectID m_Parent;
	std::vector< GameObjectID > m_Children;
};