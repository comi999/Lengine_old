#pragma once
#include <tuple>
#include <typeinfo>
#include "FunctionTraits.hpp"

//==========================================================================
using namespace std;

//==========================================================================
// Stores an invocable object or function. This includes static lambdas,
// capture lambdas, static functions and member functions with object
// instances.
//==========================================================================
template < typename Return = void, typename... Args >
class Invoker
{
public:

	template < typename Object >
	using MemberFunction     = Return( Object::* )( Args... );
	using StaticFunction     = Return( * )( Args... );
	using InvocationFunction = Return( * )( void*, void*, Args&... );
	using Signature = Return( Args... );

private:

	template < typename... T >
	struct AsInvokerImpl
	{
		using Type = Invoker< T... >;
	};

	template < typename R, typename... A >
	struct AsInvokerImpl< R, tuple< A... > >
	{
		using Type = Invoker< R, A... >;
	};

	template < typename T >
	using AsInvoker = typename AsInvokerImpl< FunctionTraits::GetReturn< T >, FunctionTraits::GetArguments< T > >::Type;

	template < typename T >
	using EnableIfInvoker = enable_if_t< is_same_v< Invoker< Return, Args... >, T >, void >;

	template < typename T >
	using DisableIfInvoker = enable_if_t< !is_same_v< Invoker< Return, Args... >, T >, void >;

public:

	/// <summary>
	/// Default constructor.
	/// </summary>
	Invoker()
		: m_Object( nullptr )
		, m_Function( nullptr )
		, m_Invocation( nullptr )
	{ }

	/// <summary>
	/// Copy constructor.
	/// </summary>
	Invoker( const Invoker< Return, Args... >& a_Invoker )
		: m_Object( a_Invoker.m_Object )
		, m_Function( a_Invoker.m_Function )
		, m_Invocation( a_Invoker.m_Invocation )
	{ }

	/// <summary>
	/// Lambda function constructor.
	/// </summary>
	template < typename Lambda, typename = FunctionTraits::EnableIfLambda< Lambda, void >, typename = DisableIfInvoker< Lambda > >
	Invoker( Lambda& a_Lambda )
		: m_Object( reinterpret_cast< void* >( &a_Lambda ) )
		, m_Function( nullptr )
		, m_Invocation( FunctorLambda< Lambda > )
	{ }

	/// <summary>
	/// Lambda function constructor.
	/// </summary>
	template < typename Lambda, typename = FunctionTraits::EnableIfLambda< Lambda, void >, typename = DisableIfInvoker< Lambda > >
	Invoker( Lambda&& a_Lambda )
		: m_Object( reinterpret_cast< void* >( &a_Lambda ) )
		, m_Function( nullptr )
		, m_Invocation( FunctorLambda< Lambda > )
	{ }

	/// <summary>
	/// Member function constructor from instance pointer.
	/// </summary>
	template < typename Object >
	Invoker( Object* a_ObjectInstance, MemberFunction< Object > a_MemberFunction )
		: m_Object( a_ObjectInstance )
		, m_Function( reinterpret_cast< void*& >( a_MemberFunction ) )
		, m_Invocation( FunctorMember< Object > )
	{ }

	/// <summary>
	/// Member function constructor from instance reference.
	/// </summary>
	template < typename Object >
	Invoker( Object& a_ObjectInstance, MemberFunction< Object > a_MemberFunction )
		: m_Object( &a_ObjectInstance )
		, m_Function( reinterpret_cast< void*& >( a_MemberFunction ) )
		, m_Invocation( FunctorMember< Object > )
	{ }

	/// <summary>
	/// Static function constructor.
	/// </summary>
	Invoker( StaticFunction a_StaticFunction )
		: m_Object( nullptr )
		, m_Function( a_StaticFunction )
		, m_Invocation( FunctorStatic )
	{ }

	/// <summary>
	/// Invoke stored function. Will return default value if not set.
	/// </summary>
	inline Return Invoke( Args... a_Args ) const
	{
		if ( !IsSet() )
		{
			return Return();
		}

		return static_cast< InvocationFunction >( m_Invocation )( m_Object, m_Function, a_Args... );
	}

	/// <summary>
	/// Invoke stored function. Will return default value if not set.
	/// </summary>
	inline Return operator()( Args... a_Args ) const
	{
		if ( !IsSet() )
		{
			return Return();
		}

		return static_cast< InvocationFunction >( m_Invocation )( m_Object, m_Function, a_Args... );
	}

	/// <summary>
	/// Check for equality between this Invoker and another Invoker of the same signature.
	/// </summary>
	inline bool operator==( const Invoker< Return, Args... >& a_Other ) const
	{
		return m_Invocation == a_Other.m_Invocation &&
			m_Object == a_Other.m_Object &&
			m_Function == a_Other.m_Function;
	}

	/// <summary>
	/// Check for equality between this Invoker's stored Lambda function and another lambda function.
	/// </summary>
	template < typename Lambda >
	inline FunctionTraits::EnableIfLambda< Lambda, bool > operator==( Lambda& a_Lambda ) const
	{
		return m_Invocation == FunctorLambda< Lambda >;
	}

	/// <summary>
	/// Check for equality between this Invoker's stored object instance and another object instance reference.
	/// </summary>
	template < typename Object >
	inline FunctionTraits::DisableIfFunction< Object, bool > operator==( const Object& a_Object ) const
	{
		return m_Object == &a_Object;
	}

	/// <summary>
	/// Check for equality between this Invoker's stored object instance and another object instance pointer.
	/// </summary>
	template < typename Object >
	inline FunctionTraits::DisableIfFunction< Object, bool > operator==( const Object* a_Object ) const
	{
		return m_Object == a_Object;
	}

	/// <summary>
	/// Check for equality between this Invoker's stored member function and another member function.
	/// </summary>
	template < typename Object >
	inline bool operator==( MemberFunction< Object > a_MemberFunction ) const
	{
		return m_Function == reinterpret_cast< void*& >( a_MemberFunction );
	}

	/// <summary>
	/// Check for equality between this Invoker's stored static function and another static function.
	/// </summary>
	inline bool operator==( StaticFunction a_StaticFunction ) const
	{
		return m_Function == a_StaticFunction;
	}

	/// <summary>
	/// Check for inequality between this Invoker and another Invoker of the same signature.
	/// </summary>
	inline bool operator!=( const Invoker< Return, Args... >& a_Other ) const
	{
		return operator==( a_Other );
	}

	/// <summary>
	/// Check for inequality between this Invoker's stored Lambda function and another lambda function.
	/// </summary>
	template < typename Lambda >
	inline FunctionTraits::EnableIfLambda< Lambda, bool > operator!=( Lambda& a_Lambda )
	{
		return !operator!=( a_Lambda );
	}

	/// <summary>
	/// Check for inequality between this Invoker's stored object instance and another object instance reference.
	/// </summary>
	template < typename Object >
	inline FunctionTraits::DisableIfFunction< Object, bool > operator!=( const Object& a_Object ) const
	{
		return m_Object == &a_Object;
	}

	/// <summary>
	/// Check for inequality between this Invoker's stored object instance and another object instance pointer.
	/// </summary>
	template < typename Object >
	inline FunctionTraits::DisableIfFunction< Object, bool > operator!=( const Object* a_Object ) const
	{
		return m_Object == &a_Object;
	}

	/// <summary>
	/// Does this invoker have a stored function?
	/// </summary>
	inline bool IsSet() const
	{
		return m_Invocation;
	}

	/// <summary>
	/// Does this invoker have a stored lambda function?
	/// </summary>
	inline bool IsLambda() const
	{
		return !m_Function;
	}

	/// <summary>
	/// Does this invoker have a stored member function?
	/// </summary>
	inline bool IsMember() const
	{
		return m_Object && m_Function;
	}

	/// <summary>
	/// Does this invoker have a stored static function?
	/// </summary>
	inline bool IsStatic() const
	{
		return !m_Object;
	}

	/// <summary>
	/// Clear the stored function.
	/// </summary>
	inline void Reset() const
	{
		m_Object = nullptr;
		m_Function = nullptr;
		m_Invocation = nullptr;
	}

private:

	/// <summary>
	/// Store lambda function invocation.
	/// </summary>
	template < typename T >
	static inline Return FunctorLambda( void* a_LambdaInstance, void*, Args&... a_Args )
	{
		return ( reinterpret_cast< T* >( a_LambdaInstance )->T::operator() )( a_Args... );
	}

	/// <summary>
	/// Store member function invocation.
	/// </summary>
	template < typename T >
	static inline Return FunctorMember( void* a_ObjectInstance, void* a_MemberFunction, Args&... a_Args )
	{
		union
		{
			void* In;
			MemberFunction< T > Out;
		} Converter;

		Converter.In = a_MemberFunction;
		return ( reinterpret_cast< T* >( a_ObjectInstance )->*Converter.Out )( a_Args... );
	}

	/// <summary>
	/// Store static function invocation.
	/// </summary>
	static inline Return FunctorStatic( void*, void* a_StaticFunction, Args&... a_Args )
	{
		return static_cast< StaticFunction >( a_StaticFunction )( a_Args... );
	}

	//==========================================================================
	template < class, class...  > friend class Delegate;
	template < class T, class U > friend auto MakeInvoker( T*, U );
	template < class T, class U > friend auto MakeInvoker( T&, U );
	template < class T          > friend auto MakeInvoker( T&    );
	template < class T          > friend auto MakeInvoker( T&&   );

	//==========================================================================
	InvocationFunction m_Invocation;
	void* m_Object;
	void* m_Function;

};

//==========================================================================
// Make invoker from given function.
//==========================================================================
template < typename T >
inline FunctionTraits::EnableIfFunction< T, Invoker<>::AsInvoker< T > > MakeInvoker( T& a_Function )
{
	return Invoker<>::AsInvoker< T >( a_Function );
}

//==========================================================================
// Make invoker from given function.
//==========================================================================
template < typename T >
inline FunctionTraits::EnableIfFunction< T, Invoker<>::AsInvoker< T > > MakeInvoker( T&& a_Function )
{
	return Invoker<>::AsInvoker< T >( a_Function );
}

//==========================================================================
// Make invoker from given function and object instance.
//==========================================================================
template < typename T, typename U >
inline FunctionTraits::EnableIfFunction< U, Invoker<>::AsInvoker< U > > MakeInvoker( T* a_Object, U a_Member )
{
	return Invoker<>::AsInvoker< U >( a_Object, a_Member );
}

//==========================================================================
// Make invoker from given function and object instance.
//==========================================================================
template < typename T, typename U >
inline FunctionTraits::EnableIfFunction< U, Invoker<>::AsInvoker< U > > MakeInvoker( T& a_Object, U a_Member )
{
	return Invoker<>::AsInvoker< U >( a_Object, a_Member );
}

/// <summary>
/// Short hand for invoker with no return type.
/// </summary>
template < typename... Args >
using Action = Invoker< void, Args... >;

/// <summary>
/// Short hand for invoker with bool return type.
/// </summary>
template < typename... Args >
using Predicate = Invoker< bool, Args... >;