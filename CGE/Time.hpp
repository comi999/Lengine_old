#pragma once
#include <chrono>
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

	inline static float GetRealDeltaTime()
	{
		return s_DeltaTime;
	}

	inline static float GetFixedTime()
	{
		return s_FixedTime;
	}

	inline static float GetFPS()
	{
		return 1.0f / s_AverageDeltaTime;
	}

private:

	friend class CGE;

	static void Tick()
	{
		static float DeltaTimes[ 100 ] { 0 };
		static int   DeltaTimeIndex = 0;
		static std::chrono::time_point PreviousTime = std::chrono::high_resolution_clock::now();
		static std::chrono::time_point CurrentTime  = std::chrono::high_resolution_clock::now();

		CurrentTime = std::chrono::high_resolution_clock::now();
		s_DeltaTime = 0.000000001f * ( CurrentTime - PreviousTime ).count();
		PreviousTime = CurrentTime;
		s_AverageDeltaTime -= DeltaTimes[ DeltaTimeIndex ] * 0.01f;
		DeltaTimes[ DeltaTimeIndex ] = s_DeltaTime;
		s_AverageDeltaTime += s_DeltaTime * 0.01f;
		DeltaTimeIndex = ++DeltaTimeIndex >= 100 ? 0 : DeltaTimeIndex;
	}
	
	inline static float s_TimeDilation     = 1.0f;
	inline static float s_DeltaTime        = 0.0f;
	inline static float s_FixedTime        = 0.01f;
	inline static float s_AverageDeltaTime = 0.0f;
};