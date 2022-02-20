#pragma once
#include <ctime>
#include "Math.hpp"

class Time
{
public:

	inline static float GetTimeDilation()
	{
		return s_TimeDilation;
	}

	inline static float SetTimeDilation( float a_TimeDilation )
	{
		s_TimeDilation = Math::Max( a_TimeDilation, 0.0f );
	}

	inline static float GetDeltaTime()
	{
		return s_DeltaTime * s_TimeDilation;
	}

	inline static float GetFixedTime()
	{
		return s_FixedTime;
	}

private:
	friend class CGE;

	static void Tick()
	{
		static time_t lastTime;
		time_t timer;
		time( &timer );
		s_DeltaTime = difftime( timer, lastTime );
		lastTime = timer;
	}
	
	static float s_TimeDilation;
	static float s_DeltaTime;
	static float s_FixedTime;
};