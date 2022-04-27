#pragma once
#include <vector>
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

public:

	T& operator[]( size_t a_Pos )
	{
		return m_Nodes[ a_Pos ].Data;
	}

	const T& operator[]( size_t a_Pos ) const
	{
		return m_Nodes[ a_Pos ].Data;
	}

	void SetRelation( size_t a_Parent, size_t a_Child )
	{
		m_Nodes[ a_Parent ].Children.push_back( &m_Nodes[ a_Child ] );
		m_Nodes[ a_Child ].Parent = &m_Nodes[ a_Parent ];
	}

	bool IsDescendant( size_t a_Descendant, size_t a_Ascendant )
	{
		return false;
	}

private:

	std::vector< Node > m_Nodes;
};