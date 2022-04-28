#pragma once
#include <vector>
#include <queue>
#include <typeindex>

template < typename T >
class UnorderedTree
{
private:
	
	struct Node
	{
		T                    Data;
		Node*                Parent;
		std::vector< Node* > Children;
	};

	template < typename T >
	class _Iterator
	{
	public:

		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

	private:

		std::queue< Node* > m_Trace;
	};

public:

	

	T& operator[]( size_t a_Pos )
	{
		return m_Nodes[ a_Pos ].Data;
	}

	const T& operator[]( size_t a_Pos ) const
	{
		return m_Nodes[ a_Pos ].Data;
	}

	void SetDescendant( size_t a_Descendant, size_t a_Ascendant )
	{
		m_Nodes[ a_Ascendant ].Children.push_back( &m_Nodes[ a_Descendant ] );
		m_Nodes[ a_Descendant ].Parent = &m_Nodes[ a_Ascendant ];
	}

	bool IsDescendant( size_t a_Descendant, size_t a_Ascendant )
	{
		Node* Ascendant  = &m_Nodes[ a_Ascendant  ];
		Node* Descendant = &m_Nodes[ a_Descendant ];

		while ( Descendant->Parent )
		{
			if ( Descendant->Parent == Ascendant )
			{
				return true;
			}

			Descendant = Descendant->Parent;
		}

		return false;
	}

	template < typename... Args >
	T& Emplace( size_t a_Parent, Args&&... a_Args )
	{
		Node& New = m_Nodes.emplace_back( { std::forward< Args >( a_Args )... }, nullptr );
		SetDescendant( m_Nodes.size() - 1, a_Parent );
		return New.Data;
	}

	template < typename... Args >
	T& Emplace( Args&&... a_Args )
	{
		return m_Nodes.emplace_back( { std::forward< Args >( a_Args )... }, nullptr ).Data;
	}

private:

	std::vector< Node* > m_Orphans;
	std::vector< Node >  m_Nodes;
};

template < typename T >
class Tree
{

};