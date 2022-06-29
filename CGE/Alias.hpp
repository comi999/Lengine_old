#pragma once
#include "Component.hpp"
#include "Name.hpp"

DefineComponent( Alias, Component )
{
public:

	const Name& GetName() const
	{
		return m_Name;
	}

	void SetName( const Name& a_Name )
	{
		m_Name = a_Name;
	}

private:

	static GameObjectID FindByName( Hash a_Name )
	{
		// Implementation required
		return GameObjectID( -1 );
	}

	friend class GameObject;
	friend class ResourcePackager;
	friend class Serialization;
	friend class Prefab;

	template < typename _Serializer >
	void Serialize( _Serializer& a_Serializer ) const
	{
		a_Serializer << m_Name;
	}

	template < typename _Deserializer >
	void Deserialize( _Deserializer& a_Deserializer )
	{
		a_Deserializer >> m_Name;
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer & m_Name;
	}


	Name m_Name;
	bool m_ToDestroy;
};