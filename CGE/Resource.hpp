#pragma once
#include "ResourcePackage.hpp"
#include "entt/resource/cache.hpp"
#include "entt/resource/handle.hpp"
#include "entt/resource/loader.hpp"
#include "Name.hpp"

// Need to add loading and unloading automatically.
class Resource;

//template < typename T >
//using ResourceHandle = entt::resource_handle< T >;

template < typename T >
class ResourceHandle
{
private:

	ResourceHandle( Hash a_Name, entt::resource_handle< T > a_Handle )
		: m_Name( a_Name )
		, m_Handle( a_Handle )
	{ }

public:

	ResourceHandle()
		: m_Name( 0 )
		, m_Handle( nullptr )
	{ }

	ResourceHandle( Hash a_Name )
		: m_Name( a_Name )
		, m_Handle( nullptr )
	{ }

	ResourceHandle( const T& a_Resource )
		: m_Name( a_Resource.GetName() )
		, m_Handle( std::make_shared< T >( a_Resource ) )
	{ }

	inline Hash GetHash() const
	{
		return m_Name;
	}

	inline bool IsLoaded() const
	{
		return static_cast< bool >( m_Handle );
	}

	inline T* Get()
	{
		return &m_Handle.get();
	}

	inline const T* Get() const
	{
		return &m_Handle.get();
	}

	inline T* Assure()
	{
		if ( !m_Name )
		{
			return nullptr;
		}

		if ( m_Handle )
		{
			return &m_Handle.get();
		}

		return Load();
	}

	inline const T* Assure() const
	{
		return const_cast< ResourceHandle< T >* >( this )->Assure();
	}

	inline T* Load()
	{
		*this = Resource::Load< T >( m_Name );
		return Get();
	}

	inline T* Reload()
	{
		Resource::Release< T >( m_Name );
		*this = Resource::Load< T >( m_Name );
		return Get();
	}

	inline bool Release()
	{
		return Resource::Release( m_Name );
		m_Handle = nullptr;
	}

	inline operator bool() const
	{
		return IsLoaded();
	}

	ResourceHandle< T >& operator=( Hash a_Name )
	{
		if ( m_Handle )
		{
			m_Handle = entt::resource_handle< T >{};
		}

		m_Name = a_Name;
		return *this;
	}

	inline T& operator*()
	{
		return *m_Handle;
	}

	inline const T& operator*() const
	{
		return *m_Handle;
	}

	inline T* operator->()
	{
		return IsLoaded() ? &m_Handle.get() : nullptr;
	}

	inline const T* operator->() const
	{
		return IsLoaded() ? &m_Handle.get() : nullptr;
	}

private:

	friend class Serialization;
	friend class ResourcePackager;
	friend class Resource;

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

	Hash                       m_Name;
	entt::resource_handle< T > m_Handle;
};

template < typename T >
using ResourceCache = entt::resource_cache< T >;

template < typename T >
class EResourceLoader : public entt::resource_loader< EResourceLoader< T >, T >
{
public:

	entt::resource_handle< T > load( Hash a_Name ) const
	{
		auto NewResource = std::make_shared< T >();
		return Resource::LoadBin( a_Name, *NewResource ) ? NewResource : entt::resource_handle< T >{};
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

	Resource() = default;

	Resource( const Name& a_Name )
		: m_Name( a_Name )
	{ }

	const Name& GetName() const
	{
		return m_Name;
	}

	void SetName( const Name& a_Name )
	{
		m_Name = a_Name;
	}

	static void Init()
	{
		s_ResourcePackage.Init( s_PackagePath );
	}

	template < typename T >
	static ResourceHandle< T > Find( Hash a_Name )
	{
		auto& Cache = s_ResourceRepository.Get< T >();
		return Cache.contains( a_Name ) ? ResourceHandle< T >{ a_Name, Cache.handle( a_Name ) } : ResourceHandle< T >{ 0, ResourceHandle< T >{} };
	}

	template < typename T >
	static ResourceHandle< T > Load( Hash a_Name )
	{
		auto& Cache = s_ResourceRepository.Get< T >();
		return Cache.contains( a_Name ) ? ResourceHandle< T >{ a_Name, Cache.handle( a_Name ) } : ResourceHandle< T >{ a_Name, Cache.load< EResourceLoader< T > >( a_Name, a_Name ) };
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
	template < typename > friend class EResourceLoader;

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

	inline static std::string        s_PackagePath = "./Resources/resource.package";
	inline static ResourcePackage    s_ResourcePackage;
	inline static ResourceRepository s_ResourceRepository;
};