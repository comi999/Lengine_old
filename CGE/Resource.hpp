#pragma once
#include <cstdint>
#include <vector>
#include <typeinfo>
#include "TypeIndexer.hpp"

class Resource;
class ResourcePool;

template < typename T >
struct ResourceRef
{
private:

	ResourceRef( T* a_Resource )
		: m_Resource( a_Resource )
	{ }

public:

	inline T* Get()
	{
		return m_Resource;
	}

	inline const T* Get() const
	{
		return m_Resource;
	}

	inline T& GetRef()
	{
		return *m_Resource;
	}

	inline const T& GetRef() const
	{
		return *m_Resource;
	}

	inline operator T*()
	{
		return m_Resource;
	}

	inline operator T* const() const
	{
		return m_Resource;
	}

	inline operator T&()
	{
		return *m_Resource;
	}

	inline operator T& const() const
	{
		return *m_Resource;
	}

private:

	friend class ResourcePool;

	T* m_Resource;
};

struct ResourcePool
{
public:

	ResourcePool()
		: m_Destructor( nullptr )
	{ }

	template < typename T >
	void Initialize()
	{
		m_Destructor = Destructor< T >;
	}

	void Reset()
	{
		for ( void* Resource : m_Resources )
		{
			m_Destructor( Resource );
		}

		m_Resources.clear();
	}

	template < typename T, typename... Args >
	ResourceRef< T > Construct( Args&&... a_Args )
	{
		m_Resources.push_back( new T( std::forward( a_Args )... ) );
		return ResourceRef< T >( reinterpret_cast< T* >( m_Resources.back() ) );
	}

private:

	template < typename T >
	static void Destructor( void* a_Instance )
	{
		delete reinterpret_cast< T* >( a_Instance );
	}
	
	void( *m_Destructor )( void* );
	std::vector< void* > m_Resources;
};

class Resource
{
private:

	typedef TypeIndexer< crc32_cpt(__FILE__) > TypeIndexer;

public:



private:

	static std::vector< ResourcePool > s_ResourcePools;
};