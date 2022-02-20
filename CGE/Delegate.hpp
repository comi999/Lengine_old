#pragma once
#include <list>
#include <vector>
#include <set>

#include "Invoker.hpp"

//==========================================================================
typedef void* DelegateHandle;

//==========================================================================
// Stores a list of invokers.
//==========================================================================
template < typename Return = void, typename... Args >
class Delegate
{
public:

	using iterator = typename list< Invoker< Return, Args... > >::iterator;
	using const_iterator = typename list< Invoker< Return, Args... > >::const_iterator;
	using InvokerType = typename Invoker< Return, Args... >;
	using DelegateType = typename Delegate< Return, Args... >;

	template < typename Object >
	using MemberFunction = Return( Object::* )( Args... );
	using StaticFunction = Return( * )( Args... );


private:

	template < typename... T >
	struct AsDelegateImpl
	{
		using Type = Delegate< T... >;
	};

	template < typename R, typename... A >
	struct AsDelegateImpl< R, tuple< A... > >
	{
		using Type = Delegate< R, A... >;
	};

	template < typename T >
	using AsDelegate = typename AsDelegateImpl< FunctionTraits::GetReturn< T >, FunctionTraits::GetArguments< T > >::Type;

public:

	/// <summary>
	/// Default constructor.
	/// </summary>
	Delegate()
		: m_IsInvoking( false )
	{ }

	/// <summary>
	/// Clear all stored invokers.
	/// </summary>
	inline void Clear()
	{
		m_Invokers.clear();
		m_ToRemove.clear();
		m_IsInvoking = false;
	}

	/// <summary>
	/// Get the count of stored invokers.
	/// </summary>
	inline size_t GetCount() const
	{
		return m_Invokers.size();
	}

	/// <summary>
	/// Is this delegate currently being invoked?
	/// </summary>
	inline bool IsInvoking() const
	{
		return m_IsInvoking;
	}

	/// <summary>
	/// Get the underlying list of invokers.
	/// </summary>
	inline const list< InvokerType >& GetInvocationList() const
	{
		return m_Invokers;
	}

	/// <summary>
	/// Invoke an invoker at a specific index.
	/// </summary>
	inline Return Invoke( size_t a_Index, Args... a_Args )
	{
		auto Iterator = m_Invokers.begin();
		advance( Iterator, a_Index );
		m_IsInvoking = true;
		Return&& Result = move( ( *Iterator )( a_Args... ) );
		m_IsInvoking = false;
		CleanUp();
		return Result;
	}

	/// <summary>
	/// Invoke an invoker with a specific handle.
	/// </summary>
	Return Invoke( DelegateHandle a_DelegateHandle, Args... a_Args )
	{
		m_IsInvoking = true;
		Return&& Result = move( ( *reinterpret_cast< InvokerType* >( a_DelegateHandle ) )( a_Args... ) );
		m_IsInvoking = false;
		CleanUp();
		return Result;
	}

	/// <summary>
	/// Invoke all stored invokers.
	/// </summary>
	void InvokeAll( Args... a_Args )
	{
		m_IsInvoking = true;

		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			( *Iterator )( a_Args... );
		}

		m_IsInvoking = false;
		CleanUp();
	}

	/// <summary>
	/// Invoke all stored invokers and store their returns in a given vector.
	/// </summary>
	void InvokeAll( vector< Return >& o_Output, Args... a_Args )
	{
		o_Output.reserve( m_Invokers.size() + o_Output.size() );

		m_IsInvoking = true;

		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			o_Output.push_back( ( *Iterator )( a_Args... ) );
		}

		m_IsInvoking = false;
		CleanUp();
	}

	/// <summary>
	/// Invoke all stored invokers.
	/// </summary>
	void operator()( Args... a_Args )
	{
		m_IsInvoking = true;

		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			( *Iterator )( a_Args... );
		}

		m_IsInvoking = false;
		CleanUp();
	}

	/// <summary>
	/// Get the invoker at a specific index.
	/// </summary>
	inline InvokerType& operator[] ( size_t a_Index )
	{
		auto Iterator = m_Invokers.begin();
		advance( Iterator, a_Index );
		return *Iterator;
	}

	/// <summary>
	/// Get the invoker with a specific handle.
	/// </summary>
	inline InvokerType& operator[] ( DelegateHandle a_DelegateHandle )
	{
		return *reinterpret_cast< InvokerType* >( a_DelegateHandle );
	}

	/// <summary>
	/// Add an invoker to the invocation list. Will return a handle for added invoker.
	/// </summary>
	inline DelegateHandle Add( const InvokerType& a_Invoker )
	{
		m_Invokers.push_back( a_Invoker );
		return reinterpret_cast< DelegateHandle >( &m_Invokers.back() );
	}

	/// <summary>
	/// Copy the invokers from another delegate to the end of invocation list.
	/// </summary>
	inline void Add( const DelegateType& a_Delegate )
	{
		m_Invokers.insert( m_Invokers.end(), a_Delegate.begin(), a_Delegate.end() );
	}

	/// <summary>
	/// Add a lambda function as an invoker to the end of the invocation list. Will return a handle
	/// for added invoker.
	/// </summary>
	template < typename Lambda >
	inline FunctionTraits::EnableIfLambda< Lambda, DelegateHandle > Add( Lambda& a_Lambda )
	{
		m_Invokers.emplace_back( a_Lambda );
		return reinterpret_cast< DelegateHandle >( &m_Invokers.back() );
	}

	/// <summary>
	/// Add a lambda function as an invoker to the end of the invocation list. Will return a handle
	/// for added invoker.
	/// </summary>
	template < typename Lambda >
	inline FunctionTraits::EnableIfLambda< Lambda, DelegateHandle > Add( Lambda&& a_Lambda )
	{
		m_Invokers.emplace_back( a_Lambda );
		return reinterpret_cast< DelegateHandle >( &m_Invokers.back() );
	}

	/// <summary>
	/// Add a member function as an invoker to the end of an invocation list. Will return a handle
	/// for added invoker.
	/// </summary>
	template < typename Object >
	inline FunctionTraits::DisableIfFunction< Object, DelegateHandle > Add( Object* a_Object, MemberFunction< Object > a_MemberFunction )
	{
		m_Invokers.emplace_back( a_Object, a_MemberFunction );
		return reinterpret_cast< DelegateHandle >( &m_Invokers.back() );
	}

	/// <summary>
	/// Add a member function as an invoker to the end of an invocation list. Will return a handle
	/// for added invoker.
	/// </summary>
	template < typename Object >
	inline FunctionTraits::DisableIfFunction< Object, DelegateHandle > Add( Object& a_Object, MemberFunction< Object > a_MemberFunction )
	{
		m_Invokers.emplace_back( a_Object, a_MemberFunction );
		return reinterpret_cast< DelegateHandle >( &m_Invokers.back() );
	}

	/// <summary>
	/// Add a static function as an invoker to the end of an invocation list. Will return a handle
	/// for added invoker.
	/// </summary>
	inline DelegateHandle Add( StaticFunction a_StaticFunction )
	{
		m_Invokers.emplace_back( a_StaticFunction );
		return reinterpret_cast< DelegateHandle >( &m_Invokers.back() );
	}

	/// <summary>
	/// Insert an invoker into the invocation list. Will return a handle for added invoker.
	/// </summary>
	DelegateHandle Insert( const iterator& a_Where, const InvokerType& a_Invoker )
	{
		return reinterpret_cast< DelegateHandle >( &*m_Invokers.insert( a_Where, a_Invoker ) );
	}

	/// <summary>
	/// Insert all invokers from a delegate into the invocation list. Will return a handle for added invoker.
	/// </summary>
	inline void Insert( const iterator& a_Where, const DelegateType& a_Delegate )
	{
		m_Invokers.insert( a_Where, a_Delegate.begin(), a_Delegate.end() );
	}

	/// <summary>
	/// Insert a lambda function into the invocation list. Will return a handle for added invoker.
	/// </summary>
	template < typename Lambda >
	inline FunctionTraits::EnableIfLambda< Lambda, DelegateHandle > Insert( const iterator& a_Where, Lambda& a_Lambda )
	{
		return reinterpret_cast< DelegateHandle >( &*m_Invokers.emplace( a_Where, a_Lambda ) );
	}

	/// <summary>
	/// Insert a lambda function into the invocation list. Will return a handle for added invoker.
	/// </summary>
	template < typename Lambda >
	inline FunctionTraits::EnableIfLambda< Lambda, DelegateHandle > Insert( const iterator& a_Where, Lambda&& a_Lambda )
	{
		return reinterpret_cast< DelegateHandle >( &*m_Invokers.emplace( a_Where, a_Lambda ) );
	}

	/// <summary>
	/// Insert a member function into the invocation list. Will return a handle for added invoker.
	/// </summary>
	template < typename Object >
	inline DelegateHandle Insert( const iterator& a_Where, Object* a_Object, MemberFunction< Object > a_MemberFunction )
	{
		return reinterpret_cast< DelegateHandle >( &*m_Invokers.emplace( a_Where, a_Object, a_MemberFunction ) );
	}

	/// <summary>
	/// Insert a member function into the invocation list. Will return a handle for added invoker.
	/// </summary>
	template < typename Object >
	inline DelegateHandle Insert( const iterator& a_Where, Object& a_Object, MemberFunction< Object > a_MemberFunction )
	{
		return reinterpret_cast< DelegateHandle >( &*m_Invokers.emplace( a_Where, a_Object, a_MemberFunction ) );
	}

	/// <summary>
	/// Insert a static function into the invocation list. Will return a handle for added invoker.
	/// </summary>
	inline DelegateHandle Insert( const iterator& a_Where, StaticFunction a_StaticFunction )
	{
		return reinterpret_cast< DelegateHandle >( &*m_Invokers.emplace( a_Where, a_StaticFunction ) );
	}

	/// <summary>
	/// Safely remove an invoker at a given index. Will not be removed until delegate has finished invoking.
	/// </summary>
	bool Remove( size_t a_Index )
	{
		if ( a_Index >= m_Invokers.size() )
		{
			return false;
		}

		auto Iterator = m_Invokers.begin();
		advance( Iterator, a_Index );

		if ( m_IsInvoking )
		{
			m_ToRemove.push_back( Iterator );
		}
		else
		{
			m_Invokers.erase( Iterator );
		}

		return true;
	}

	/// <summary>
	/// Safely remove a matching invoker. Will not be removed until delegate has finished invoking.
	/// </summary>
	bool Remove( const InvokerType& a_Invoker )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( *Iterator == a_Invoker )
			{
				if ( m_IsInvoking )
				{
					m_ToRemove.push_back( Iterator );
				}
				else
				{
					m_Invokers.erase( Iterator );
				}

				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Safely remove an invoker by handle. Will not be removed until delegate has finished invoking.
	/// </summary>
	bool Remove( DelegateHandle a_DelegateHandle )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_DelegateHandle )
			{
				if ( m_IsInvoking )
				{
					m_ToRemove.push_back( Iterator );
				}
				else
				{
					m_Invokers.erase( Iterator );
				}

				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Safely remove an invoker at a given position. Will not be removed until delegate has finished invoking.
	/// </summary>
	bool Remove( const iterator& a_Where )
	{
		if ( m_IsInvoking )
		{
			m_ToRemove.push_back( a_Where );
		}
		else
		{
			m_Invokers.erase( a_Where );
		}

		return true;
	}

	/// <summary>
	/// Safely remove an invoker containing a given lambda. Will not be removed until delegate has finished invoking.
	/// </summary>
	template < typename Lambda >
	FunctionTraits::EnableIfLambda< Lambda, bool > Remove( Lambda& a_Lambda )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_Lambda )
			{
				if ( m_IsInvoking )
				{
					m_ToRemove.push_back( Iterator );
				}
				else
				{
					m_Invokers.erase( Iterator );
				}

				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Safely remove an invoker containing the given object instance. Will not be removed until delegate has finished invoking.
	/// </summary>
	template < typename Object >
	FunctionTraits::DisableIfFunction< Object, bool > Remove( const Object& a_Object )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_Object )
			{
				if ( m_IsInvoking )
				{
					m_ToRemove.push_back( Iterator );
				}
				else
				{
					m_Invokers.erase( Iterator );
				}

				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Safely remove an invoker containing the given object instance. Will not be removed until delegate has finished invoking.
	/// </summary>
	template < typename Object >
	FunctionTraits::DisableIfFunction< Object, bool > Remove( const Object* a_Object )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_Object )
			{
				if ( m_IsInvoking )
				{
					m_ToRemove.push_back( Iterator );
				}
				else
				{
					m_Invokers.erase( Iterator );
				}

				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Safely remove an invoker containing the given static function. Will not be removed until delegate has finished invoking.
	/// </summary>
	bool Remove( StaticFunction a_StaticFunction )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_StaticFunction )
			{
				if ( m_IsInvoking )
				{
					m_ToRemove.push_back( Iterator );
				}
				else
				{
					m_Invokers.erase( Iterator );
				}

				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Remove an invoker at a given index.
	/// </summary>
	bool ForceRemove( size_t a_Index )
	{
		if ( a_Index >= m_Invokers.size() )
		{
			return false;
		}

		auto Iterator = m_Invokers.begin();
		advance( Iterator, a_Index );
		m_Invokers.erase( Iterator );
		return true;
	}

	/// <summary>
	/// Remove a matching invoker.
	/// </summary>
	bool ForceRemove( const InvokerType& a_Invoker )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( *Iterator == a_Invoker )
			{
				m_Invokers.erase( Iterator );
				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Remove an invoker by handle.
	/// </summary>
	bool ForceRemove( DelegateHandle a_DelegateHandle )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_DelegateHandle )
			{
				m_Invokers.erase( Iterator );
				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Remove an invoker at a given position.
	/// </summary>
	void ForceRemove( const iterator& a_Where )
	{
		m_Invokers.erase( a_Where );
	}

	/// <summary>
	/// Safely remove an invoker containing a given lambda. Will not be removed until delegate has finished invoking.
	/// </summary>
	template < typename Lambda >
	FunctionTraits::EnableIfLambda< Lambda, bool > ForceRemove( Lambda& a_Lambda )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_Lambda )
			{
				m_Invokers.erase( Iterator );
				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Safely remove an invoker containing the given object instance. Will not be removed until delegate has finished invoking.
	/// </summary>
	template < typename Object >
	FunctionTraits::DisableIfFunction< Object, bool > ForceRemove( const Object& a_Object )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_Object )
			{
				m_Invokers.erase( Iterator );
				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Safely remove an invoker containing the given object instance. Will not be removed until delegate has finished invoking.
	/// </summary>
	template < typename Object >
	FunctionTraits::DisableIfFunction< Object, bool > ForceRemove( const Object* a_Object )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_Object )
			{
				m_Invokers.erase( Iterator );
				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Safely remove an invoker containing the given static function. Will not be removed until delegate has finished invoking.
	/// </summary>
	bool ForceRemove( StaticFunction a_StaticFunction )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_StaticFunction )
			{
				m_Invokers.erase( Iterator );
				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Remove all invokers that contain given invoker.
	/// </summary>
	template < typename Lambda >
	FunctionTraits::EnableIfLambda< Lambda, void > RemoveAll( Lambda& a_Lambda )
	{
		if ( m_IsInvoking )
		{
			for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
			{
				if ( *Iterator == a_Lambda )
				{
					m_ToRemove.push_back( Iterator );
				}
			}
		}
		else
		{
			m_Invokers.remove_if( [&]( const InvokerType& a_Invoker ) { return a_Invoker == a_Lambda; } );
		}
	}

	/// <summary>
	/// Remove all invokers that contain given object instance.
	/// </summary>
	template < typename Object >
	FunctionTraits::DisableIfFunction< Object, void > RemoveAll( const Object* a_Object )
	{
		if ( m_IsInvoking )
		{
			for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
			{
				if ( *Iterator == a_Object )
				{
					m_ToRemove.push_back( Iterator );
				}
			}
		}
		else
		{
			m_Invokers.remove_if( [&]( const InvokerType& a_Invoker )
				{
					return a_Invoker == a_Object;
				} );
		}
	}

	/// <summary>
	/// Remove all invokers that contain given object instance.
	/// </summary>
	template < typename Object >
	FunctionTraits::DisableIfFunction< Object, void > RemoveAll( const Object& a_Object )
	{
		if ( m_IsInvoking )
		{
			for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
			{
				if ( *Iterator == a_Object )
				{
					m_ToRemove.push_back( Iterator );
				}
			}
		}
		else
		{
			m_Invokers.remove_if( [&]( const InvokerType& a_Invoker )
				{
					return a_Invoker == a_Object;
				} );
		}
	}

	/// <summary>
	/// Remove all invokers that contain given member function.
	/// </summary>
	template < typename Object >
	bool RemoveAll( MemberFunction< Object > a_MemberFunction )
	{
		if ( m_IsInvoking )
		{
			for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
			{
				if ( *Iterator == a_MemberFunction )
				{
					m_ToRemove.push_back( Iterator );
				}
			}
		}
		else
		{
			m_Invokers.remove_if( [&]( const InvokerType& a_Invoker )
				{
					return a_Invoker == a_MemberFunction;
				} );
		}
	}

	/// <summary>
	/// Remove all invokers that contain given static function.
	/// </summary>
	void RemoveAll( StaticFunction a_StaticFunction )
	{
		if ( m_IsInvoking )
		{
			for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
			{
				if ( *Iterator == a_StaticFunction )
				{
					m_ToRemove.push_back( Iterator );
				}
			}
		}
		else
		{
			m_Invokers.remove_if( [&]( const InvokerType& a_Invoker )
				{
					return a_Invoker == a_StaticFunction;
				} );
		}
	}

	/// <summary>
	/// Add an invoker onto the end of the invocation list.
	/// </summary>
	void operator+=( const InvokerType& a_Invoker )
	{
		m_Invokers.push_back( a_Invoker );
	}

	/// <summary>
	/// Add all invokers in a delegate onto the end of the invocation list.
	/// </summary>
	void operator+=( const DelegateType& a_Delegate )
	{
		m_Invokers.insert( m_Invokers.end(), a_Delegate.begin(), a_Delegate.end() );
	}

	/// <summary>
	/// Add a lambda function onto the end of the invocation list.
	/// </summary>
	template < typename Lambda >
	FunctionTraits::EnableIfLambda< Lambda, void > operator+=( Lambda& a_Lambda )
	{
		m_Invokers.emplace_back( a_Lambda );
	}

	/// <summary>
	/// Add a lambda function onto the end of the invocation list.
	/// </summary>
	template < typename Lambda >
	FunctionTraits::EnableIfLambda< Lambda, void > operator+=( Lambda&& a_Lambda )
	{
		m_Invokers.emplace_back( a_Lambda );
	}

	/// <summary>
	/// Add a static function onto the end of the invocation list.
	/// </summary>
	void operator+=( StaticFunction a_StaticFunction )
	{
		m_Invokers.emplace_back( a_StaticFunction );
	}

	/// <summary>
	/// Remove an invoker at a given index from the invocation list.
	/// </summary>
	void operator-=( size_t a_Index )
	{
		auto Iterator = m_Invokers.begin();
		advance( Iterator, a_Index );
		m_Invokers.erase( Iterator );
	}

	/// <summary>
	/// Remove an invoker with a given handle from the invocation list.
	/// </summary>
	void operator-=( DelegateHandle a_DelegateHandle )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_DelegateHandle )
			{
				m_Invokers.erase( Iterator );
				return;
			}
		}
	}

	/// <summary>
	/// Remove an invoker at a given position from the invocation list.
	/// </summary>
	void operator-=( const iterator& a_Where )
	{
		m_Invokers.erase( a_Where );
	}

	/// <summary>
	/// Remove an invoker that matches a given invoker from the invocation list.
	/// </summary>
	void operator-=( const InvokerType& a_Invoker )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( *Iterator == a_Invoker )
			{
				m_Invokers.erase( Iterator );
				return;
			}
		}
	}
	
	/// <summary>
	/// Remove an invoker containing given lambda from the invocation list.
	/// </summary>
	template < typename Lambda >
	FunctionTraits::EnableIfLambda< Lambda, void > operator-=( Lambda& a_Lambda )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( *Iterator == a_Lambda )
			{
				m_Invokers.erase( Iterator );
				return;
			}
		}
	}

	/// <summary>
	/// Remove an invoker containing given object instance from the invocation list.
	/// </summary>
	template < typename Object >
	FunctionTraits::DisableIfFunction< Object, void > operator-=( const Object& a_Object )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( *Iterator == a_Object )
			{
				m_Invokers.erase( Iterator );
				return;
			}
		}
	}

	/// <summary>
	/// Remove an invoker containing given object instance from the invocation list.
	/// </summary>
	template < typename Object >
	FunctionTraits::DisableIfFunction< Object, void > operator-=( const Object* a_Object )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( *Iterator == a_Object )
			{
				m_Invokers.erase( Iterator );
				return;
			}
		}
	}

	/// <summary>
	/// Remove an invoker containing given member function from the invocation list.
	/// </summary>
	template < typename Object >
	void operator-=( MemberFunction< Object > a_MemberFunction )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( *Iterator == a_MemberFunction )
			{
				m_Invokers.erase( Iterator );
				return;
			}
		}
	}

	/// <summary>
	/// Remove an invoker containing given static function from the invocation list.
	/// </summary>
	void operator-=( StaticFunction a_StaticFunction )
	{
		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( *Iterator == a_StaticFunction )
			{
				m_Invokers.erase( Iterator );
				return;
			}
		}
	}

	/// <summary>
	/// Get Handle at given index. Will be an invalid handle if index is out of range.
	/// </summary>
	DelegateHandle GetHandle( size_t a_Index ) const
	{
		auto Iterator = GetIterator( a_Index );
		return Iterator != m_Invokers.end() ? &*Iterator : nullptr;
	}

	/// <summary>
	/// Get Handle at given position.
	/// </summary>
	DelegateHandle GetHandle( iterator a_Where ) const
	{
		return &*a_Where;
	}

	/// <summary>
	/// Is the given handle valid in this delegate. Time complexity is O(n).
	/// </summary>
	inline bool IsHandleValid( DelegateHandle a_DelegateHandle ) const
	{
		if ( !a_DelegateHandle )
		{
			return false;
		}

		for ( auto Iterator = m_Invokers.begin(); Iterator != m_Invokers.end(); ++Iterator )
		{
			if ( &*Iterator == a_DelegateHandle )
			{
				return true;
			}
		}

		return false;
	}

	/// <summary>
	/// Begin iterator.
	/// </summary>
	inline iterator begin()
	{
		return m_Invokers.begin();
	}

	/// <summary>
	/// End iterator.
	/// </summary>
	inline iterator end()
	{
		return m_Invokers.end();
	}

	/// <summary>
	/// Begin iterator.
	/// </summary>
	inline const_iterator begin() const
	{
		return m_Invokers.begin();
	}

	/// <summary>
	/// End iterator.
	/// </summary>
	inline const_iterator end() const
	{
		return m_Invokers.end();
	}

private:

	/// <summary>
	/// Clean up after invocation.
	/// </summary>
	void CleanUp()
	{
		for ( auto Iterator = m_ToRemove.begin(); Iterator != m_ToRemove.end(); ++Iterator )
		{
			m_Invokers.erase( *Iterator );
		}

		m_ToRemove.clear();
	}

	iterator GetIterator( size_t a_Index )
	{
		if ( a_Index <= m_Invokers.size() * 0.5f )
		{
			auto Iterator = m_Invokers.begin();
			advance( Iterator, a_Index );
			return Iterator;
		}
		else if ( a_Index < m_Invokers.end() )
		{
			auto Iterator = m_Invokers.end();
			advance( Iterator, static_cast< uint64_t >( m_Invokers.size() - a_Index ) );
			return Iterator;
		}

		return m_Invokers.end();
	}

	template < class T, class U > friend auto MakeDelegate( T*, U );
	template < class T, class U > friend auto MakeDelegate( T&, U );
	template < class T          > friend auto MakeDelegate( T&    );
	template < class T          > friend auto MakeDelegate( T&&   );

	//==========================================================================
	list< InvokerType > m_Invokers;
	list< iterator >    m_ToRemove;
	bool                m_IsInvoking;

};

//==========================================================================
// Make delegate from given function.
//==========================================================================
template < typename T >
FunctionTraits::EnableIfFunction< T, Delegate<>::AsDelegate< T > > MakeDelegate( T& a_Function )
{
	Delegate<>::AsDelegate< T > Result;
	Result.Add( a_Function );
	return Result;
}

//==========================================================================
// Make delegate from given function.
//==========================================================================
template < typename T >
FunctionTraits::EnableIfFunction< T, Delegate<>::AsDelegate< T > > MakeDelegate( T&& a_Function )
{
	Delegate<>::AsDelegate< T > Result;
	Result.Add( a_Function );
	return Result;
}

//==========================================================================
// Make delegate from given function and object instance.
//==========================================================================
template < typename T, typename U >
FunctionTraits::EnableIfFunction< U, Delegate<>::AsDelegate< U > > MakeDelegate( T* a_Object, U a_Member )
{
	Delegate<>::AsDelegate< U > Result;
	Result.Add( a_Object, a_Member );
	return Result;
}

//==========================================================================
// Make delegate from given function and object instance.
//==========================================================================
template < typename T, typename U >
FunctionTraits::EnableIfFunction< U, Delegate<>::AsDelegate< U > > MakeDelegate( T& a_Object, U a_Member )
{
	Delegate<>::AsDelegate< U > Result;
	Result.Add( a_Object, a_Member );
	return Result;
}