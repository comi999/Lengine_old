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