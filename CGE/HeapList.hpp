#pragma once
#include <vector>

template < typename T >
class LinkedArray
{
//private:
	typedef std::pair< size_t, size_t > Location;

	inline static Location FindLocation( size_t a_Index )
	{
		size_t HeapIndex;
		HeapIndex = !!BitScanReverse64( &HeapIndex, a_Index + 1 ) * HeapIndex;
		return Location( HeapIndex, a_Index + 1 - ( static_cast< size_t >( 1 ) << HeapIndex ) );
	}
};