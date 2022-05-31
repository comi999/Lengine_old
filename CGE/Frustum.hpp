#pragma once
#include "Math.hpp"
#include "Camera.hpp"

class Frustum
{
public:

	static Frustum GenerateFrustum( const Camera& a_Camera )
	{
		// Create frustum object;
	}

private:

	union
	{
		Plane Planes[ 6 ];
		struct
		{
			Plane Left;
			Plane Right;
			Plane Top;
			Plane Bottom;
			Plane Front;
			Plane Back;
		};
	};
};