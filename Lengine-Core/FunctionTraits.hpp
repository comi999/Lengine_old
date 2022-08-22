#pragma once
#include <tuple>

//==========================================================================
// Utilities for determining traits of function types.
//==========================================================================
namespace FunctionTraits
{
	namespace // Private
	{
		template < typename T >
		struct InfoImpl;

		template < typename R, typename... A >
		struct InfoImpl< R( A... ) >
		{
			using Signature = R( A... );
			using Return    = R;
			using Arguments = std::tuple< A... >;
			static constexpr bool IsStatic   = true;
			static constexpr bool IsLambda   = false;
			static constexpr bool IsMember   = false;
			static constexpr bool IsFunction = true;
		};

		template < typename O, typename R, typename... A >
		struct InfoImpl< R( O::* )( A... ) >
		{
			using Signature = R( A... );
			using Return    = R;
			using Arguments = std::tuple< A... >;
			static constexpr bool IsStatic   = false;
			static constexpr bool IsLambda   = false;
			static constexpr bool IsMember   = true;
			static constexpr bool IsFunction = true;
		};

		template < typename O, typename R, typename... A >
		struct InfoImpl< R( O::* )( A... ) const >
		{
			using Signature = R( A... );
			using Return    = R;
			using Arguments = std::tuple< A... >;
			static constexpr bool IsStatic   = false;
			static constexpr bool IsLambda   = false;
			static constexpr bool IsMember   = true;
			static constexpr bool IsFunction = true;
		};

		template < typename O, typename R, typename... A >
		struct InfoImpl< R( O::* )( A... ) volatile >
		{
			using Signature = R( A... );
			using Return = R;
			using Arguments = std::tuple< A... >;
			static constexpr bool IsStatic   = false;
			static constexpr bool IsLambda   = false;
			static constexpr bool IsMember   = true;
			static constexpr bool IsFunction = true;
		};

		template < typename O, typename R, typename... A >
		struct InfoImpl< R( O::* )( A... ) const volatile >
		{
			using Signature = R( A... );
			using Return    = R;
			using Arguments = std::tuple< A... >;
			static constexpr bool IsStatic   = false;
			static constexpr bool IsLambda   = false;
			static constexpr bool IsMember   = true;
			static constexpr bool IsFunction = true;
		};

		template < typename R, typename... A >
		struct InfoImpl< R( * )( A... ) >
		{
			using Signature = R( A... );
			using Return    = R;
			using Arguments = std::tuple< A... >;
			static constexpr bool IsStatic   = true;
			static constexpr bool IsLambda   = false;
			static constexpr bool IsMember   = false;
			static constexpr bool IsFunction = true;
		};

		template < typename R, typename... A >
		struct InfoImpl< R( & )( A... ) >
		{
			using Signature = R( A... );
			using Return    = R;
			using Arguments = std::tuple< A... >;
			static constexpr bool IsStatic   = true;
			static constexpr bool IsLambda   = false;
			static constexpr bool IsMember   = false;
			static constexpr bool IsFunction = true;
		};

		template < typename T >
		struct InfoImpl
		{
			using Signature = void;
			using Return = void;
			using Arguments = std::tuple<>;
			static constexpr bool IsStatic   = false;
			static constexpr bool IsLambda   = false;
			static constexpr bool IsMember   = false;
			static constexpr bool IsFunction = false;
		};

		template < typename T >
		struct HasInvocationOperator
		{
			template < typename U > static char test( decltype( &U::operator() ) );
			template < typename U > static long test( ... );
			static constexpr bool Value = sizeof( test< T >( 0 ) ) == sizeof( char );
		};

		template < typename T >
		struct LambdaInfoImpl
		{
			using Signature = decltype( &std::remove_reference< T >::type::operator() );
			using Return    = typename InfoImpl< Signature >::Return;
			using Arguments = typename InfoImpl< Signature >::Arguments;
			static constexpr bool IsStatic   = false;
			static constexpr bool IsLambda   = true;
			static constexpr bool IsMember   = false;
			static constexpr bool IsFunction = true;
		};
	}

	template < typename T >
	struct Info : public std::conditional_t< HasInvocationOperator< T >::Value, LambdaInfoImpl< T >, InfoImpl< T > >
	{ };

	template < typename T, typename O >
	using EnableIfFunction = std::enable_if_t< Info< T >::IsFunction, O >;

	template < typename T, typename O >
	using DisableIfFunction = std::enable_if_t< !Info< T >::IsFunction, O >;

	template < typename T, typename O >
	using EnableIfStatic = std::enable_if_t< Info< T >::IsStatic, O >;

	template < typename T, typename O >
	using DisableIfStatic = std::enable_if_t< !Info< T >::IsStatic, O >;

	template < typename T, typename O >
	using EnableIfMember = std::enable_if_t< Info< T >::IsMember, O >;

	template < typename T, typename O >
	using DisableIfMember = std::enable_if_t< !Info< T >::IsMember, O >;

	template < typename T, typename O >
	using EnableIfLambda = std::enable_if_t< Info< T >::IsLambda, O >;

	template < typename T, typename O >
	using DisableIfLambda = std::enable_if_t< !Info< T >::IsLambda, O >;

	template < typename T, size_t INDEX >
	using GetArgument = std::tuple_element_t< INDEX, typename Info< T >::Arguments >;

	template < typename T >
	using GetArguments = typename Info< T >::Arguments;

	template < typename T >
	using GetReturn = typename Info< T >::Return;

	template < typename T >
	using GetSignature = typename Info< T >::Signature;

	template < typename T >
	static constexpr size_t GetArgumentCount = std::tuple_size_v< Info< T >::Arguments >;

	template < typename T >
	static constexpr bool IsFunction = Info< T >::IsFunction;

	template < typename T >
	static constexpr bool IsStatic = Info< T >::IsStatic;

	template < typename T >
	static constexpr bool IsMember = Info< T >::IsMember;

	template < typename T >
	static constexpr bool IsLambda = Info< T >::IsLambda;
} // namespace FunctionTraits