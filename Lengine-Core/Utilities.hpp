#pragma once
#include <tuple>

struct OnStart
{
	template < typename _Callable >
	OnStart( _Callable a_Callable ) { a_Callable(); }
};

template < typename... T >
struct combine_tuple
{};

template < typename... T, typename... U >
struct combine_tuple< std::tuple< T... >, std::tuple< U... > >
{
	using Tuple = std::tuple< T..., U... >;
};

template < typename T >
struct unwrap
{
	using Tuple = std::tuple<>;
};

template < template < typename... > class T, typename U >
struct unwrap< T< U > > : public unwrap< U >
{
	using Tuple = typename combine_tuple< std::tuple< T< void > >, typename unwrap< U >::Tuple >::Tuple;
};

template < template < typename... > class T >
struct unwrap< T< void > >
{
	using Tuple = std::tuple< T< void > >;
};

//template < typename... T >
//struct get_super_impl
//{
//	using type = void;
//};
//
//template < typename... T >
//struct get_super_impl< std::tuple< T... > >
//{
//	template < typename... U >
//	static std::enable_if_t< ( sizeof...( U ) > 1 ), std::tuple_element_t< sizeof...( U ) - 2, std::tuple< U... > > >
//		test( ... );
//
//	template < typename... U >
//	static std::enable_if_t< !( sizeof...( U ) > 1 ), void >
//		test( ... );
//
//	using type = decltype( test< T... >( 0 ) );
//};
//
//template < typename T >
//using get_super = typename get_super_impl< T >::type;
//
//using t = get_super< std::tuple< int, double, float > >;