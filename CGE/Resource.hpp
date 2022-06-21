#pragma once
#include "ResourcePackage.hpp"
#include "entt/resource/cache.hpp"
#include "entt/resource/handle.hpp"
#include "entt/resource/loader.hpp"
#include "Hash.hpp"

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

	ResourceHandle< T > load( const std::string a_Name ) const
	{
		auto NewResource = std::make_shared< T >();
		return Resource::LoadBin( a_Name, *NewResource ) ? NewResource : nullptr;
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

	template < typename T >
	static ResourceHandle< T > Find( std::string a_Name )
	{
		Hash Name = CRC32_RT( a_Name.c_str() );
		auto& Cache = s_ResourceRepository.Get< T >();
		return Cache.contains( Name ) ? Cache.handle( Name ) : ResourceHandle< T >();
	}

	template < typename T >
	static ResourceHandle< T > FindOrLoad( std::string a_Name )
	{
		Hash Name = CRC32_RT( a_Name.c_str() );
		auto& Cache = s_ResourceRepository.Get< T >();
		return Cache.load< ResourceLoader< T > >( Name, a_Name );
	}

	template < typename T >
	static T* Get( std::string a_Name )
	{
		Hash Name = CRC32_RT( a_Name.c_str() );
		auto& Cache = s_ResourceRepository.Get< int >();
		return Cache.contains( Name ) ? &*Cache.handle( Name ) : nullptr;
	}

	template < typename T >
	inline static T* GetOrLoad( std::string a_Name )
	{
		return &*s_ResourceRepository.Get< T >().load< ResourceLoader< T > >( CRC32_RT( a_Name.c_str() ), a_Name );
	}

	template < typename T >
	inline static bool Loaded( std::string a_Name )
	{
		return s_ResourceRepository.Get< T >().contains( CRC32_RT( a_Name.c_str() ) );
	}

	template < typename T >
	static bool Unload( std::string a_Name )
	{
		Hash Name = CRC32_RT( a_Name.c_str() );
		auto& Cache = s_ResourceRepository.Get< int >();

		if ( !Cache.contains( Name ) )
		{
			return false;
		}

		Cache.discard( Name );
		return true;
	}

private:

	template < typename T >
	inline static bool LoadBin( const std::string& a_Name, T& o_Resource )
	{
		return s_ResourcePackage.Load( o_Resource, a_Name );
	}

	friend class CGE;
	template < typename > friend class ResourceLoader;

	inline static ResourcePackage    s_ResourcePackage = File( "./Resources/resource.package" );
	inline static ResourceRepository s_ResourceRepository;
};