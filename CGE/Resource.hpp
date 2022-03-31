#pragma once
#include <cstdint>
#include <vector>
#include <typeinfo>
#include "TypeIndexer.hpp"

class Resource;

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