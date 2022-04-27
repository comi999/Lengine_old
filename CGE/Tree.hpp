//#include <vector>
//#include <array>
//#include <queue>
//#include <map>
//
//using namespace std;
//
//template < typename _Type, size_t _To, size_t _From >
//struct GraphNode
//{
//	template < size_t _Size = size_t( -1 ) >
//	using IOType = conditional_t< _Size == size_t( -1 ), vector< size_t >, array< size_t, _Size > >;
//	using ValueType = _Type;
//	using NodeType = GraphNode< _Type, _To, _From >;
//
//	_Type           Data;
//	IOType< _To   > To;
//	IOType< _From > From;
//};
//
//template < typename _Type, typename _Node = GraphNode< _Type, size_t( -1 ), size_t( -1 ) > >
//class Graph
//{
//	static_assert( is_same_v< _Type, typename _Node::ValueType >, "Graph type and node type do not match." );
//
//	using NodeType  = typename _Node;
//	using ValueType = typename _Type;
//	using Iterator  = typename vector< NodeType >::iterator;
//	using CIterator = typename vector< NodeType >::const_iterator;
//
//public:
//
//	inline Iterator Begin()
//	{
//		return m_Nodes.begin();
//	}
//
//	inline CIterator Begin() const
//	{
//		return m_Nodes.begin();
//	}
//
//	inline Iterator End()
//	{
//		return m_Nodes.end();
//	}
//
//	inline CIterator End() const
//	{
//		return m_Nodes.end();
//	}
//
//	template < typename... Args >
//	Iterator Emplace( Args&&... a_Args )
//	{
//		m_Nodes.emplace_back( { forward< Args >( a_Args )... } );
//		return m_Nodes.end() - 1;
//	}
//
//	
//private:
//
//	vector< NodeType > m_Nodes;
//};
//
//
//template < typename _Type >
//class Tree;
//
//template < typename _Tree >
//class TreeIterator;
//
//template < typename _Tree >
//class ConstTreeIterator;
//
//template < typename _Type >
//struct TreeNode
//{
//	using ValueType = _Type;
//	using NodeType  = TreeNode< _Type >;
//
//	_Type               Data;
//	size_t              Layer;
//	NodeType*           Parent;
//	vector< NodeType* > Children;
//};
//
//template < typename _Tree >
//class TreeIterator
//{
//public:
//
//	using iterator_category = forward_iterator_tag;
//	using value_type        = typename _Tree::ValueType;
//	using difference_type   = ptrdiff_t;
//	using pointer           = value_type*;
//	using reference         = value_type&;
//	using TreeType          = _Tree;
//	using NodeType          = typename _Tree::NodeType;
//	using IteratorType      = typename _Tree::Iterator;
//	using CIteratorType     = typename _Tree::CIterator;
//
//private:
//
//	TreeIterator( TreeType* a_Tree, NodeType* a_Node )
//		: m_Tree( a_Tree )
//	{
//		m_Trace.push( a_Node );
//	}
//
//	template < typename It >
//	TreeIterator( TreeType* a_Tree, It a_Begin, It a_End )
//		: m_Tree( a_Tree )
//	{
//		for ( ; a_Begin != a_End; ++a_Begin )
//		{
//			m_Trace.push( *a_Begin );
//		}
//	}
//
//	void Verify()
//	{
//		_STL_ASSERT( !m_Trace.empty(), "Cannot iterate past end." );
//	}
//
//	void Advance()
//	{
//		Verify();
//
//		NodeType* Front = m_Trace.front();
//		m_Trace.pop();
//		
//		for ( auto Begin = Front->Children.begin(), End = Front->Children.end(); Begin != End; ++Begin )
//		{
//			m_Trace.push( *Begin );
//		}
//	}
//
//public:
//
//	TreeIterator()
//		: m_Tree( nullptr )
//	{ }
//
//	TreeIterator( const IteratorType& a_Iterator ) = default;
//
//	auto& operator++()
//	{
//		Advance();
//		return *this;
//	}
//
//	auto operator++( int )
//	{
//		IteratorType Temp{ *this };
//		Advance();
//		return move( Temp );
//	}
//	queue< int >
//	auto& operator*()
//	{
//		return m_Trace.front()->Data;
//	}
//
//	auto& operator*() const
//	{
//		return m_Trace.front()->Data;
//	}
//
//	auto operator->()
//	{
//		return &m_Trace.front()->Data;
//	}
//
//	auto operator->() const
//	{
//		return &m_Trace.front()->Data;
//	}
//
//	inline bool operator==( const IteratorType& a_Iterator ) const
//	{
//		return &m_Trace.front() == &a_Iterator.m_Trace.front();
//	}
//
//	inline bool operator!=( const IteratorType& a_Iterator ) const
//	{
//		return !( *this == a_Iterator );
//	}
//
//private:
//
//	TreeType*          m_Tree;
//	queue< NodeType* > m_Trace;
//};
//
//template < typename _Tree >
//class ConstTreeIterator
//{
//
//};
//
//template < typename _Type >
//class Tree
//{
//private:
//
//	template < typename > friend class TreeNode;
//	template < typename > friend class TreeIterator;
//
//	using ValueType = _Type;
//	using TreeType  = Tree< ValueType >;
//	using NodeType  = TreeNode< ValueType >;
//	using Iterator  = TreeIterator< TreeType >;
//	using CIterator = TreeIterator< const TreeType >;
//
//	void Verify( const NodeType* a_Node )
//	{
//		_STL_ASSERT( a_Node - m_Nodes.data() < m_Nodes.size(), "Node out of range." );
//	}
//
//public:
//
//	auto Begin()
//	{
//		return 
//	}
//
//	auto& End()
//	{
//		static const Iterator Iter;
//		return Iter;
//	}
//
//	auto Find( const _Type& a_Value )
//	{
//		Iterator Begin( m_Orphans.begin(), m_Orphans.end() );
//		Iterator End = Iterator();
//
//		while ( Begin != End )
//		{
//			if ( *Begin == a_Value )
//			{
//				break;
//			}
//
//			++Begin;
//		}
//
//		return move( Begin );
//	}
//
//	CIterator Find( const _Type& a_Value ) const
//	{
//
//	}
//
//private:
//
//	vector< NodeType* > m_Orphans;
//	vector< NodeType  > m_Nodes;
//};