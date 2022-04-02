#pragma once
#include <typeinfo>
#include <vector>
#include "Indexer.hpp"

class TypeMap
{
	typedef Indexer< "TypeMap"_H > TypeMapIndexer;
	typedef std::type_info TypeInfo;

	struct Function
	{
		template < typename Return, typename... Args >
		Function( Return( *a_Function )( Args... ) )
			: m_Function( a_Function )
		{ }

		template < typename Return, typename... Args >
		Return Invoke( Args... a_Args )
		{
			return static_cast< Return( * )( Args... ) >( m_Function )( std::forward< Args >( a_Args )... );
		}

	private:

		void* m_Function;
	};
	
	struct Node
	{
		const TypeInfo*         TypeInfo;
		std::vector< Function > Functions;
		std::vector< Node* >    Children;
	};

	template < typename T >
	static Node* CreateNode()
	{
		size_t Index = Indexer< "TypeMap"_H >::Instance< HashType< "Node"_H > >::GetIndex< T >();

		if ( Index < s_Nodes.size() )
		{
			return nullptr;
		}

		&s_Nodes.emplace_back( { &typeid( T ) } );
	}

	template < typename T, auto F >
	static void ConnectFunction()
	{
		size_t Index = Indexer< "TypeMap"_H >::Instance< HashType< "Node"_H > >::GetIndex< T >();
		
	}

public:

	TypeMap() = delete;

private:

	static std::vector< Node > s_Nodes;
};