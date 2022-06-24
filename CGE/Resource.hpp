#pragma once
#include "ResourcePackage.hpp"
#include "entt/resource/cache.hpp"
#include "entt/resource/handle.hpp"
#include "entt/resource/loader.hpp"
#include "Name.hpp"

// Need to add loading and unloading automatically.
class Resource;

template < typename T >
using ResourceHandle = entt::resource_handle< T >;

template < typename T >
using ResourceCache = entt::resource_cache< T >;

template < typename T >
class ResourceLoader : public entt::resource_loader< ResourceLoader< T >, T >
{
public:

	ResourceHandle< T > load( Hash a_Name ) const
	{
		auto NewResource = std::make_shared< T >();
		return Resource::LoadBin( a_Name, *NewResource ) ? NewResource : ResourceHandle< T >{};
	}
};

class ResourceRepository
{
private:

	static size_t Next()
	{
		static size_t Current = 0;
		return Current++;
	}

	template < typename T >
	static size_t Index()
	{
		static size_t Current = Next();
		return Current;
	}

	template < typename T >
	ResourceCache< T >& Assure()
	{
		static size_t Current = Index< T >();

		if ( !( Current < m_ResourceCaches.size() ) )
		{
			m_ResourceCaches.resize( Current + 1u );
		}

		if ( !m_ResourceCaches[ Current ].first )
		{
			m_ResourceCaches[ Current ].first = new ResourceCache< T >();
			m_ResourceCaches[ Current ].second = CacheDeleter< T >;
		}

		return *reinterpret_cast< ResourceCache< T >* >( m_ResourceCaches[ Current ].first );
	}

public:

	~ResourceRepository()
	{
		for ( auto Begin = m_ResourceCaches.begin(), End = m_ResourceCaches.end(); Begin != End; ++Begin )
		{
			if ( Begin->first )
			{
				Begin->second( Begin->first );
			}
		}
	}

	template < typename T >
	ResourceCache< T >& Get()
	{
		return Assure< T >();
	}

private:

	template < typename T >
	static void CacheDeleter( void* a_Cache )
	{
		delete reinterpret_cast< T* >( a_Cache );
	}

	std::vector< std::pair< void*, void( * )( void* ) > > m_ResourceCaches;
};

class Resource
{
public:

	const Name& GetName() const
	{
		return m_Name;
	}

	static void Init( const std::string& a_Package )
	{
		s_ResourcePackage.Init( a_Package );
	}

	template < typename T >
	static ResourceHandle< T > Find( Hash a_Name )
	{
		auto& Cache = s_ResourceRepository.Get< T >();
		return Cache.contains( a_Name ) ? Cache.handle( a_Name ) : ResourceHandle< T >{};
	}

	template < typename T >
	static ResourceHandle< T > Load( Hash a_Name )
	{
		auto& Cache = s_ResourceRepository.Get< T >();
		return Cache.contains( a_Name ) ? Cache.handle( a_Name ) : Cache.load< ResourceLoader< T > >( a_Name, a_Name );
	}

	template < typename T >
	static bool Release( Hash a_Name )
	{
		auto& Cache = s_ResourceRepository.Get< T >();

		if ( !Cache.contains( a_Name ) )
		{
			return false;
		}

		Cache.discard( a_Name );
		return true;
	}

	template < typename T >
	inline static bool IsLoaded( Hash a_Name )
	{
		return s_ResourceRepository.Get< T >().contains( a_Name );
	}

private:

	template < typename T >
	inline static bool LoadBin( Hash a_Name, T& o_Resource )
	{
		return s_ResourcePackage.Load( o_Resource, a_Name );
	}

	friend class CGE;
	friend class Serialization;
	friend class ResourcePackager;
	template < typename > friend class ResourceLoader;

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << m_Name;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> m_Name;
	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{
		a_Sizer & m_Name;
	}

	Name m_Name;

	inline static ResourcePackage    s_ResourcePackage;
	inline static ResourceRepository s_ResourceRepository;
};