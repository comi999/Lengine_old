#pragma once

template < typename T, typename Allocator >
class LinkedArray
{
private:

	inline pair< size_t, size_t > GetLocation( size_t a_Index ) const
	{
		auto QSQRT = []( float a_Value )
		{
			long i;
			float x2, y;
			const float threehalfs = 1.5F;

			x2 = a_Value * 0.5F;
			y = a_Value;
			i = *( long* )&y;                       
			i = 0x5f3759df - ( i >> 1 );               
			y = *( float* )&i;
			y = y * ( threehalfs - ( x2 * y * y ) );   
			//	y  = y * ( threehalfs - ( x2 * y * y ) );   

			return y;
		};


	}
};