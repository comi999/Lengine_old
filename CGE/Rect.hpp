#pragma once
#include "Math.hpp"

template < typename T >
struct TRect
{
	Vector< T, 2 > Origin;
	Vector< T, 2 > Size;

	inline T GetLeft() const
	{
		return Origin.x;
	}

	inline T GetRight() const
	{
		if constexpr ( std::is_integral_v< T > ) 
		{
			return Origin.x + Size.x - 1;
		}
		else                         
		{
			return Origin.x + Size.x;
		}
	}

	inline T GetBottom() const
	{
		return Origin.y;
	}

	inline T GetTop() const
	{
		if constexpr ( std::is_integral_v< T > )
		{
			return Origin.y + Size.y - 1;
		}
		else
		{
			return Origin.y + Size.y;
		}
	}

	template < typename U, size_t S >
	bool Contains( const Vector< U, S >& a_Position ) const
	{
		if ( 
			a_Position[ 0 ] < GetLeft() ||
			a_Position[ 0 ] > GetRight() ||
			a_Position[ 1 ] < GetBottom() ||
			a_Position[ 1 ] > GetTop() )
		{
			return false;
		}

		return true;
	}
};

typedef TRect< float    > Rect;
typedef TRect< int32_t  > RectInt;
typedef TRect< uint32_t > RectUInt;