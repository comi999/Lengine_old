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

	inline Prefab* AddChild()
	{
		return &m_Children.emplace_back();
	}

	bool DestroyChild( uint32_t a_Index )
	{
		if ( !( a_Index < m_Children.size() ) )
		{
			return false;
		}

		m_Children.erase( m_Children.begin() + a_Index );
		return true;
	}

	inline const Prefab* GetChild( uint32_t a_Index ) const
	{
		return ( a_Index < m_Children.size() ) ? &m_Children[ a_Index ] : nullptr;
	}

	inline Prefab* GetChild( uint32_t a_Index )
	{
		return const_cast< Prefab* >( this )->GetChild( a_Index );
	}

	inline size_t GetChildCount() const
	{
		return m_Children.size();
	}

	template < typename _Component >
	inline bool HasComponent() const
	{
		return m_Components.find( typeid( _Component ) ) != m_Components.end();
	}

	template < typename _Component >
	bool AddComponent( const _Component& a_Component )
	{
		static size_t ComponentHash = typeid( std::remove_const_t< _Component > ).hash_code();
		auto Iter = m_Components.find( ComponentHash );

		if ( Iter != m_Components.end() )
		{
			return false;
		}

		auto& Binary = m_Components[ ComponentHash ];
		Binary.resize( Serialization::GetSizeOf( a_Component ) );
		bool Successful;
		BufferStream Stream;
		Stream.Open( const_cast< uint8_t* >( Binary.data() ), Binary.size() );
		BufferSerializer Serializer( Stream );
		ComponentBase::GetTypeMap().InvokeFunction( ComponentHash, "BufferSerializeComponent"_H, Successful, Serializer, &a_Component );
		Stream.Close();
		return Successful;
	}

	template < typename _Component >
	bool AddComponent()
	{
		static size_t ComponentHash = typeid( std::remove_const_t< _Component > ).hash_code();
		auto Iter = m_Components.find( ComponentHash );

		if ( Iter != m_Components.end() )
		{
			return false;
		}

		auto& Binary = m_Components[ ComponentHash ];
		_Component NewComponent;
		Binary.resize( Serialization::GetSizeOf( NewComponent ) );
		bool Successful;
		BufferStream Stream;
		Stream.Open( const_cast< uint8_t* >( Binary.data() ), Binary.size() );
		BufferSerializer Serializer( Stream );
		ComponentBase::GetTypeMap().InvokeFunction( ComponentHash, "BufferSerializeComponent"_H, Successful, Serializer, &ThisComponent );
		Stream.Close();
		return Successful;
	}

	template < typename _Component >
	bool DestroyComponent()
	{
		static size_t ComponentHash = typeid( std::remove_const_t< _Component > ).hash_code();
		auto Iter = m_Components.find( ComponentHash );

		if ( Iter == m_Components.end() )
		{
			return false;
		}

		m_Components.erase( Iter );
		return true;
	}

	template < typename _Component >
	bool PatchComponent( const Action< _Component& >& a_Patch )
	{
		static size_t ComponentHash = typeid( std::remove_const_t< _Component > ).hash_code();
		auto Iter = m_Components.find( ComponentHash );

		if ( Iter == m_Components.end() )
		{
			return false;
		}

		_Component ThisComponent;
		bool Successful;
		BufferStream Stream;
		Stream.Open( const_cast< uint8_t* >( Iter->second.data() ), Iter->second.size() );
		BufferDeserializer Deserializer( Stream );
		ComponentBase::GetTypeMap().InvokeFunction( ComponentHash, "BufferDeserializeComponent"_H, Successful, Deserializer, &ThisComponent );
		Stream.Close();

		if ( !Successful )
		{
			return false;
		}

		a_Patch( ThisComponent );
		Iter->second.resize( Serialization::GetSizeOf( ThisComponent ) );
		Stream.Open();
		BufferSerializer Serializer( Stream );
		ComponentBase::GetTypeMap().InvokeFunction( ComponentHash, "BufferSerializeComponent"_H, Successful, Serializer, &ThisComponent );
		Stream.Close();
		return Successful;
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