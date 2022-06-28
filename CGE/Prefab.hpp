#pragma once
#include "Resource.hpp"
#include "GameObject.hpp"

// remove these, just for tests
#include "MeshRenderer.hpp"
#include "Transform.hpp"

class Prefab : public Resource
{
public:

	Prefab() = default;

	static GameObject Instantiate( const Prefab& a_Prefab )
	{
		// Create GameObject
		GameObject NewGameObject = GameObject::Instantiate( a_Prefab.GetName() );

		// Add Components to GameObject
		for ( auto Begin = a_Prefab.m_Components.begin(), End = a_Prefab.m_Components.end(); Begin != End; ++Begin )
		{
			// Add component with the given type hash.
			void* NewComponent;
			ComponentBase::GetTypeMap().InvokeFunction( Begin->first, "AddComponent"_H, NewComponent, NewGameObject.GetID() );
			
			// Deserialize prefab data into component.
			bool Successful;
			BufferStream Stream;
			Stream.Open( const_cast< uint8_t* >( Begin->second.data() ), Begin->second.size() );
			BufferDeserializer Deserializer( Stream );
			ComponentBase::GetTypeMap().InvokeFunction( Begin->first, "BufferDeserializeComponent"_H, Successful, Deserializer, NewComponent );
		}

		// Create sub prefabs and attach as children recursively.
		for ( auto Begin = a_Prefab.m_Children.begin(), End = a_Prefab.m_Children.end(); Begin != End; ++Begin )
		{
			GameObject NewChild = Prefab::Instantiate( *Begin );
			NewGameObject.GetTransform()->AttachChild( NewChild, false );
		}

		NewGameObject.GetTransform()->UpdateTransform();
		return NewGameObject;
	}

private:

	friend class ResourcePackager;
	friend class Serialization;

	typedef std::vector< uint8_t > ComponentBinary;
	typedef std::map< size_t, ComponentBinary > ComponentBinaryMap;

	template < typename _Serializer >
	void Serialize( _Serializer& a_Serializer ) const
	{
		a_Serializer << reinterpret_cast< const Resource& >( *this ) << m_Components << m_Children;
	}

	template < typename _Deserializer >
	void Deserialize( _Deserializer& a_Deserializer )
	{
		a_Deserializer >> reinterpret_cast< Resource& >( *this ) >> m_Components >> m_Children;
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer & reinterpret_cast< const Resource& >( *this ) & m_Components & m_Children;
	}

	ComponentBinaryMap    m_Components;
	std::vector< Prefab > m_Children;
};