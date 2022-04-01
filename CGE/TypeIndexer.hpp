#pragma once
#include "Hash.hpp"

template < Hash IndexerName >
class TypeIndexer
{
public:

	static size_t Next()
	{
		static size_t Index = 0;
		return Index++;
	}

	template < typename T >
	static size_t GetTypeIndex()
	{
		static size_t Index = Next();
		return Index;
	}
};

typedef TypeIndexer< "ResourceTypeIndexer"_H > ResourceTypeIndexer;
#include <vector>
void foo()
{
	size_t intIndex = ResourceTypeIndexer::GetTypeIndex< int >();
	size_t intIndex1 = ResourceTypeIndexer::GetTypeIndex< int >();
	size_t floatIndex = ResourceTypeIndexer::GetTypeIndex< float >();
}

std::vector< void* > resources;

template < typename T, typename... Args >
void CreateResource( Args&&... args )
{
	int index = ResourceTypeIndexer::GetTypeIndex< T >();

	if ( index >= resources.size() )
	{
		resources.resize( index + 1 );
	}

	resources[ index ] = new T( std::forward( args )... );
}

template < typename T >
T* GetResource()
{
	int index = ResourceTypeIndexer::GetTypeIndex< T >();

	if ( index >= resources.size() )
	{
		return nullptr;
	}

	return reinterpret_cast< T* >( resources[ index ] );
}