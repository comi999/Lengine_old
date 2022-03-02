#pragma once
#include <type_traits>
#include "Component.hpp"
#include "FunctionTraits.hpp"

//#define DefineHasFunction( Name, Index, Signature )                                                                                                                        \
//template < typename T >                                                                                                                                                    \
//struct Has##Name##_##Index                                                                                                                                                 \
//{                                                                                                                                                                          \
//	template < typename U, typename = std::enable_if_t< std::is_same_v< FunctionTraits::GetSignature< decltype( &U::##Name ) >, ##Signature > > > static char test( int ); \
//	template < typename U > static long test( ... );																													   \
//	static constexpr bool Value = sizeof( test< T >( 0 ) ) == sizeof( char );																							   \
//}                                                                                                                                                                          \
//
//class MonoBehaviour : protected Component
//{
//public:
//
//	inline GameObject& GetGameObject()
//	{
//		return Component::GetGameObject();
//	}
//
//private:
//
//	DefineHasFunction( Awake, 0, void() );
//	DefineHasFunction( Awake, 1, void( StartupParams& ) );
//	DefineHasFunction( Start, 0, void() );
//	DefineHasFunction( Update, 0, void() );
//	DefineHasFunction( FixedUpdate, 0, void() );
//
//	template < typename T >
//	inline static auto RunAwake( T& a_Instance )
//	{
//		if constexpr ( HasAwake_0< T >::Value )
//		{
//			a_Instance.Awake();
//			return;
//		}
//
//		if constexpr ( HasAwake_1< T >::Value )
//		{
//			StartupParams Params;
//			a_Instance.Awake( Params );
//			return;
//		}
//	}
//	
//	template < typename T >
//	inline static auto RunStart( T& a_Instance )
//	{
//		if constexpr ( HasStart_0< T >::Value )
//		{
//			a_Instance.Start();
//			return;
//		}
//	}
//
//	template < typename T >
//	inline static auto RunUpdate( T& a_Instance )
//	{
//		if constexpr ( HasUpdate_0< T >::Value )
//		{
//			a_Instance.Update();
//			return;
//		}
//	}
//
//	template < typename T >
//	inline static auto RunFixedUpdate( T& a_Instance )
//	{
//		if constexpr ( HasUpdate_0< T >::Value )
//		{
//			a_Instance.Update();
//			return;
//		}
//	}
//
//};
//
//struct StartupParams
//{
//	bool Ticking = false;
//};
//
//class TestScript : MonoBehaviour
//{
//public:
//
//	void Awake( StartupParams& Params )
//	{
//		Params.Ticking = true;
//	}
//
//};