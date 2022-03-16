#pragma once
#include <array>
#include <map>
#include <list>
#include <stack>
#include <queue>
#include <typeinfo>
#include "Hash.hpp"
#include "Invoker.hpp"

typedef std::type_info TypeInfo;

class TypeMap
{
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
		Node( const TypeInfo& a_TypeInfo )
			: m_TypeInfo( a_TypeInfo )
		{
		}

		const TypeInfo& m_TypeInfo;
		std::vector< Node* > m_Children;
		std::map< Hash, Function > m_FunctionTable;
	};

	struct BFIterator
	{
	private:

		BFIterator( const Node* a_Node )
		{
			m_Trace.push( a_Node );
		}

	public:

		using iterator_category = std::forward_iterator_tag;
		using value_type = const TypeInfo;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;


		BFIterator() = default;

		BFIterator& operator++()
		{
			_STL_VERIFY( !m_Trace.empty(), "Cannot iterate past end." );

			const Node* Current = m_Trace.front();
			m_Trace.pop();

			for ( auto Begin = Current->m_Children.begin(), End = Current->m_Children.end(); Begin != End; ++Begin )
			{
				m_Trace.push( *Begin );
			}

			return *this;
		}

		BFIterator operator++( int )
		{
			_STL_VERIFY( !m_Trace.empty(), "Cannot iterate past end." );

			BFIterator Temp = *this;
			++* this;
			return Temp;
		}

		inline const TypeInfo& operator*() const
		{
			_STL_VERIFY( !m_Trace.empty(), "Cannot dereference end iterator." );

			return m_Trace.front()->m_TypeInfo;
		}

		inline const TypeInfo* operator->() const
		{
			_STL_VERIFY( !m_Trace.empty(), "Cannot dereference end iterator." );

			return &m_Trace.front()->m_TypeInfo;
		}

		inline bool operator==( const BFIterator& a_Iterator ) const
		{
			if ( m_Trace.empty() || a_Iterator.m_Trace.empty() )
			{
				return m_Trace.empty() && a_Iterator.m_Trace.empty();
			}

			return m_Trace.front() == a_Iterator.m_Trace.front();
		}

		inline bool operator!=( const BFIterator& a_Iterator ) const
		{
			return !( *this == a_Iterator );
		}

	private:

		friend class TypeMap;

		std::queue< const Node* > m_Trace;
	};

	struct DFIterator
	{
	private:

		DFIterator( const Node* a_Node )
		{
			m_Trace.push( a_Node );
		}

	public:

		using iterator_category = std::forward_iterator_tag;
		using value_type = const TypeInfo;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		DFIterator() = default;

		DFIterator& operator++()
		{
			_STL_VERIFY( !m_Trace.empty(), "Cannot iterate past end." );

			const Node* Current = m_Trace.top();
			m_Trace.pop();

			for ( auto Begin = Current->m_Children.begin(), End = Current->m_Children.end(); Begin != End; ++Begin )
			{
				m_Trace.push( *Begin );
			}

			return *this;
		}

		DFIterator operator++( int )
		{
			_STL_VERIFY( !m_Trace.empty(), "Cannot iterate past end." );

			DFIterator Temp = *this;
			++* this;
			return Temp;
		}

		inline const TypeInfo& operator*() const
		{
			_STL_VERIFY( !m_Trace.empty(), "Cannot dereference end iterator." );

			return m_Trace.top()->m_TypeInfo;
		}

		inline const TypeInfo* operator->() const
		{
			_STL_VERIFY( !m_Trace.empty(), "Cannot dereference end iterator." );

			return &m_Trace.top()->m_TypeInfo;
		}

		inline bool operator==( const DFIterator& a_Iterator ) const
		{
			if ( m_Trace.empty() || a_Iterator.m_Trace.empty() )
			{
				return m_Trace.empty() && a_Iterator.m_Trace.empty();
			}

			return m_Trace.top() == a_Iterator.m_Trace.top();
		}

		inline bool operator!=( const DFIterator& a_Iterator ) const
		{
			return !( *this == a_Iterator );
		}

	private:

		friend class TypeMap;

		std::stack< const Node* > m_Trace;
	};

public:

	template < typename This, typename Base = void >
	bool RegisterType()
	{
		auto type1 = typeid( This ).name();
		auto type2 = typeid( Base ).name();

		auto Found = m_Nodes.find( typeid( This ).hash_code() );

		if ( Found == m_Nodes.end() )
		{
			const TypeInfo& ThisInfo = typeid( This );
			auto* NewNode = &m_Nodes.emplace( std::make_pair( ThisInfo.hash_code(), Node( ThisInfo ) ) ).first->second;

			// No parent.
			if constexpr ( std::is_same_v< Base, void > )
			{
				return true;
			}

			auto NewFound = m_Nodes.find( typeid( Base ).hash_code() );

			if ( NewFound == m_Nodes.end() )
			{
				const TypeInfo& BaseInfo = typeid( Base );
				m_Nodes.emplace( std::make_pair( BaseInfo.hash_code(), Node( BaseInfo ) ) ).first->second.m_Children.push_back( NewNode );
			}
			else
			{
				NewFound->second.m_Children.push_back( NewNode );
			}

			return true;
		}

		return false;
	}

	template < typename Tuple >
	inline bool RegisterTypes()
	{
		using First = typename Tuple::_This_type;
		using Base = typename Tuple::_Mybase;

		if constexpr ( std::is_same_v< Base, std::tuple<> > )
		{
			return RegisterType< First >();
		}
		else
		{
			if ( RegisterType< typename Base::_This_type, First >() )
			{
				return RegisterTypes< Base >();
			}
		}

		return false;
	}

	template < typename T, typename Return, typename... Args >
	inline bool RegisterFunction( Hash a_Name, Return( *a_Function )( Args... ) )
	{
		auto Found = m_Nodes.find( typeid( T ).hash_code() );

		if ( Found != m_Nodes.end() )
		{
			Found->second.m_FunctionTable.emplace( a_Name, Function( a_Function ) );
			return true;
		}

		return false;
	}

	template < typename Return, typename... Args >
	inline bool InvokeFunction( size_t a_HashCode, Hash a_Name, Return& a_Return, Args... a_Args )
	{
		auto Found = m_Nodes.find( a_HashCode );

		if ( Found != m_Nodes.end() )
		{
			auto FoundFunction = Found->second.m_FunctionTable.find( a_Name );

			if ( FoundFunction != Found->second.m_FunctionTable.end() )
			{
				a_Return = FoundFunction->second.Invoke< Return, Args... >( std::forward< Args >( a_Args )... );
				return true;
			}
		}

		return false;
	}

	inline auto GetBFBegin() const
	{
		if ( !m_Nodes.size() )
		{
			return BFIterator();
		}

		return BFIterator( &m_Nodes.begin()->second );
	}

	template < typename T >
	inline auto GetBFBegin() const
	{
		auto Found = m_Nodes.find( typeid( T ).hash_code() );

		if ( Found == m_Nodes.end() )
		{
			return BFIterator();
		}

		return BFIterator( &Found->second );
	}

	inline auto GetBFEnd() const
	{
		return BFIterator();
	}

	inline auto GetDFBegin() const
	{
		if ( !m_Nodes.size() )
		{
			return DFIterator();
		}

		return DFIterator( &m_Nodes.begin()->second );
	}

	template < typename T >
	inline auto GetDFBegin() const
	{
		auto Found = m_Nodes.find( typeid( T ).hash_code() );

		if ( Found == m_Nodes.end() )
		{
			return DFIterator();
		}

		return DFIterator( &Found->second );
	}

	inline auto GetDFEnd() const
	{
		return DFIterator();
	}

	inline auto GetTypes() const
	{
		std::list< const TypeInfo* > Result;

		for ( auto Begin = GetBFBegin(), End = GetBFEnd(); Begin != End; ++Begin )
		{
			Result.push_back( &*Begin );
		}

		return Result;
	}

	template < typename T >
	inline auto GetTypes() const
	{
		auto Found = m_Nodes.find( typeid( T ).hash_code() );
		std::list< const TypeInfo* > Result;

		if ( Found != m_Nodes.end() )
		{
			for ( auto Begin = GetBFBegin< T >(), End = GetBFEnd(); Begin != End; ++Begin )
			{
				Result.push_back( &*Begin );
			}

			return Result;
		}

		return Result;
	}

private:

	std::map< size_t, Node > m_Nodes;
};