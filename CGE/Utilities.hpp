#pragma once

struct OnStart
{
	template < typename _Callable >
	OnStart( _Callable a_Callable ) { a_Callable(); }
};