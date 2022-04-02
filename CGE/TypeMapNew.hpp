#pragma once
#include <typeinfo>
#include <vector>
#include "Indexer.hpp"

class TypeMap
{
	typedef Indexer< TypeMap, "TypeMap"_H > Indexer;
	typedef std::type_info TypeInfo;
	
	struct Node
	{
		const TypeInfo*      TypeInfo;
		std::vector< Node* > Children;
	};

	template < typename T >
	static Node CreateNode()
	{

	}

	template < typename T, typename Base >
	static Node CreateNode()
	{

	}

public:

	TypeMap() = delete;

private:

	static std::vector< Node > s_Nodes;
};