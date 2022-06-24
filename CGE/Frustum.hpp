#pragma once
#include "Math.hpp"
#include "Camera.hpp"

struct Frustum
{
	Frustum( const Camera& a_Camera )
	{
		const Transform* CameraTransform = a_Camera.GetOwner().GetTransform();
		Vector2 FarSize;
		FarSize.y = a_Camera.GetFarZ() * Math::Tan( a_Camera.GetFOV() * 0.5f );
		FarSize.x = FarSize.y * a_Camera.GetAspect();
		Vector3 CameraPosition = CameraTransform->GetGlobalPosition();
		Vector3 CameraForward = CameraTransform->GetGlobalForward();
		Vector3 CameraRight = CameraTransform->GetGlobalRight();
		Vector3 CameraUp = CameraTransform->GetGlobalUp();
		Vector3 NearP = a_Camera.GetNearZ() * CameraForward;
		Vector3 FarP = a_Camera.GetFarZ() * CameraForward;

		// Front
		Front.normal = CameraForward;
		Front.distance = Math::Dot( CameraPosition + NearP, Front.normal );

		// Back
		Back.normal = -CameraForward;
		Back.distance = Math::Dot( CameraPosition + FarP, Back.normal );

		// Right
		Right.normal = Math::Cross( CameraUp, FarP + CameraRight * FarSize.x );
		Right.distance = Math::Dot( CameraPosition, Right.normal );

		// Left
		Left.normal = Math::Cross( CameraUp, FarP - CameraRight * FarSize.x );
		Left.distance = Math::Dot( CameraPosition, Left.normal );

		// Top
		Top.normal = Math::Cross( CameraRight, FarP - CameraUp * FarSize.y );
		Top.distance = Math::Dot( CameraPosition, Top.normal );

		// Bottom
		Bottom.normal = Math::Cross( FarP + CameraUp * FarSize.y, CameraRight );
		Bottom.distance = Math::Dot( CameraPosition, Bottom.normal );
	}

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