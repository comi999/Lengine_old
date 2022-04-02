#pragma once
#include "Hash.hpp"

template < auto H > struct HashType {};

template < Hash Instance >
class Indexer
{
public:

	template < typename T >
	struct Instance
	{
		static size_t TypeNext()
		{
			static size_t Index = 0;
			return Index++;
		}

		static size_t HashNext()
		{
			static size_t Index = 0;
			return Index++;
		}

		template < typename T >
		static size_t GetIndex()
		{
			static size_t Index = TypeNext();
			return Index;
		}

		template < Hash H >
		static size_t GetIndex()
		{
			static size_t Index = HashNext();
			return Index;
		}
	};
};