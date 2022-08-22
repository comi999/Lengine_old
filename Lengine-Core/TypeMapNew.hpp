#pragma once
#include <typeinfo>
#include <vector>
#include <queue>
#include <stack>
#include "Hash.hpp"
#include "entt/entt.hpp"

//entt::registry a;
//class TypeMap
//{
//	typedef std::type_info TypeInfo;
//	template < auto H > struct HashType {};
//
//	template < typename... T >
//	struct Sequencer
//	{
//		static size_t Next()
//		{
//			static size_t Index = 0;
//			return Index++;
//		}
//	};
//
//	template < typename... T >
//	struct TypeSequencer : public Sequencer< T... >
//	{
//		template < typename Type >
//		static size_t Index()
//		{
//			static size_t Value = Next();
//			return Value;
//		}
//	};
//
//	template < typename... T >
//	struct LiteralSequencer : public Sequencer< T... >
//	{
//		template < auto Literal >
//		static size_t Index()
//		{
//			static size_t Value = Next();
//			return Value;
//		}
//	};
//
//	enum class SearchType
//	{
//		BreadthFirst,
//		DepthFirst,
//	};
//
//	struct Node
//	{
//	public:
//
//		template < Hash H, typename Return, typename... Args >
//		Return Invoke( Args&&... a_Args )
//		{
//			typedef Return( *Function )( Args... );
//			static size_t FunctionIndex = LiteralSequencer< HashType< "Function"_H > >::Index< H >();
//			_STL_ASSERT( FunctionIndex < Functions.size() && Functions[ FunctionIndex ], "No function of that name exists." );
//			return reinterpret_cast< Function >( Functions[ FunctionIndex ] )( std::forward< Args >( a_Args )... );
//		}
//
//		const TypeInfo& GetTypeInfo() const
//		{
//			return *TypeInfo;
//		}
//
//	private:
//
//		template < SearchType > friend class Iterator;
//		friend class TypeMap;
//
//		const TypeInfo*      TypeInfo;
//		Node*                Parent;
//		std::vector< Node* > Children;
//		std::vector< void* > Functions;
//	};
//
//	template < SearchType S >
//	class Iterator
//	{
//	private:
//
//		Iterator( const Node* a_Node )
//		{
//			Push( a_Node );
//		}
//
//		Iterator( const std::vector< const Node* >& a_Nodes )
//		{
//			for ( const Node* Orphan : a_Nodes )
//			{
//				Push( Orphan );
//			}
//		}
//
//		const Node* Pop()
//		{
//			if constexpr ( S == SearchType::BreadthFirst )
//			{
//				const Node* _Node = m_Trace.front();
//				m_Trace.pop();
//				return _Node;
//			}
//
//			if constexpr ( S == SearchType::DepthFirst )
//			{
//				const Node* _Node = m_Trace.top();
//				m_Trace.pop();
//				return _Node;
//			}
//		}
//
//		const Node* Peek() const
//		{
//			if constexpr ( S == SearchType::BreadthFirst )
//			{
//				return m_Trace.front();
//			}
//
//			if constexpr ( S == SearchType::DepthFirst )
//			{
//				return m_Trace.top();
//			}
//		}
//
//		void Push( const Node* a_Node )
//		{
//			m_Trace.push( a_Node );
//		}
//
//	public:
//
//		using iterator_category = std::forward_iterator_tag;
//		using value_type = const TypeInfo;
//		using difference_type = std::ptrdiff_t;
//		using pointer = value_type*;
//		using reference = value_type&;
//
//		Iterator() = default;
//
//		Iterator& operator++()
//		{
//			_STL_VERIFY( !m_Trace.empty(), "Cannot iterate past end." );
//			const Node* Current = Pop();
//
//			for ( auto Begin = Current->Children.begin(), End = Current->Children.end(); Begin != End; ++Begin )
//			{
//				Push( *Begin );
//			}
//
//			return *this;
//		}
//
//		Iterator operator++( int )
//		{
//			_STL_VERIFY( !m_Trace.empty(), "Cannot iterate past end." );
//			Iterator Temp = *this;
//			const Node* Current = Pop();
//
//			for ( auth Begin = Current->Children.begin(), End = Current->Children.end(); Begin != End; ++Begin )
//			{
//				Push( *Begin );
//			}
//
//			return Temp;
//		}
//
//		inline const Node& operator*() const
//		{
//			_STL_VERIFY( !m_Trace.empty(), "Cannot dereference end." );
//			return *Peek();
//		}
//
//		inline const TypeInfo* operator->() const
//		{
//			_STL_VERIFY( !m_Trace.empty(), "Cannot dereference end." );
//			return Peek();
//		}
//
//		inline bool operator==( const Iterator< S > & a_Iterator ) const
//		{
//			if ( m_Trace.empty() || a_Iterator.m_Trace.empty() )
//			{
//				return m_Trace.empty() && a_Iterator.m_Trace.empty();
//			}
//
//			return Peek() == a_Iterator.Peek();
//		}
//
//		inline bool operator!=( const BFIterator & a_Iterator ) const
//		{
//			return !( *this == a_Iterator );
//		}
//
//	private:
//
//		friend class TypeMap;
//
//		std::conditional_t< S == SearchType::BreadthFirst, std::queue< Node* >, std::stack< Node* > > m_Trace;
//	};
//
//	template < typename T >
//	Node* AssureNode()
//	{
//		static size_t TypeIndex = TypeSequencer< HashType< "Node"_H > >::Index< T >();
//
//		if ( !( Index < m_Nodes.size() ) )
//		{
//			m_Nodes.resize( Index + 1u );
//		    Node* Back = &m_Nodes.back();
//			Back->TypeInfo = &typeid( T );
//			return Back;
//		}
//
//		return &m_Nodes.back();
//	}
//
//	template < typename T, Hash H >
//	void*& AssureFunction()
//	{
//		static size_t Index = LiteralSequencer< HashType< "Function"_H > >::Index< H >();
//		Node* _Node = AssureNode< T >();
//
//		if ( !( Index < _Node->Functions.size() ) )
//		{
//			_Node->Functions.resize( Index + 1u );
//		}
//
//		return _Node->Functions.back();
//	}
//	
//	template < typename T, Hash H, typename Return, typename... Args >
//	void* GetFunction()
//	{
//		static size_t NodeIndex = TypeSequencer< HashType< "Node"_H > >::Index< T >();
//		static size_t FunctionIndex = LiteralSequencer< HashType< "Function"_H >, T >::Index< H >();
//
//		if ( !( NodeIndex < m_Nodes.size() ) )
//		{
//			return nullptr;
//		}
//
//		Node* _Node = &m_Nodes[ NodeIndex ];
//		
//		if ( !( FunctionIndex < _Node->Functions.size() ) )
//		{
//			return nullptr;
//		}
//
//		return _Node->Functions[ FunctionIndex ];
//	}
//
//public:
//
//	template < typename T, typename Base, typename... Rest >
//	inline void Link()
//	{
//		Node* ChildNode    = AssureNode< T >();
//		Node* BaseNode     = AssureNode< Base >();
//		size_t ChildIndex = TypeSequencer< HashType< "Children"_H >, Base >::Index< T >();
//
//		if ( !( ChildIndex < BaseNode->Children.size() ) )
//		{
//			BaseNode->Children.resize( ChildIndex + 1u );
//		}
//
//		BaseNode->Children[ ChildIndex ] = ChildNode;
//		ChildNode->Parent = BaseNode;
//
//		if constexpr ( sizeof...( Rest ) > 0 )
//		{
//			Link< Base, Rest... >();
//		}
//	}
//
//	template < typename T, Hash H, typename Return, typename... Args >
//	inline void Connect( Return( *a_Function )( Args... ) )
//	{
//		AssureFunction< T, H >() = a_Function;
//	}
//
//	template < typename T, Hash H, typename Return, typename... Args >
//	inline Return Invoke( Args&&... a_Args ) const
//	{
//		typedef Return( *Function )( Args... );
//		Function _Function = reinterpret_cast< Function >( GetFunction< T, H, Return, Args... >() );
//		_ASSERT_EXPR( _Function != nullptr, "No function exists with that name for that type." );
//		return _Function( std::forward< Args >( a_Args )... );
//	}
//
//	template < SearchType S >
//	Iterator< S > Begin() const
//	{
//		return Iterator< S >( m_Orphans );
//	}
//
//	template < SearchType S >
//	Iterator< S > End() const
//	{
//		return Iterator< S >();
//	}
//
//private:
//
//	std::vector< Node >        m_Nodes;
//	std::vector< const Node* > m_Orphans;
//};