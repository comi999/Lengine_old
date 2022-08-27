#pragma once

#include "Component.hpp"
#include "Input.hpp"
#include "Transform.hpp"
#include "GameObject.hpp"
#include "Math.hpp"

DefineComponent( CameraController, Component )
{
public:

	float MovementSpeed = 0.1f;
	float RotationSpeed = 0.1f;

	void Update()
	{
		Transform* CameraTransform = Camera::GetMainCamera()->GetTransform();

		if ( Input::IsKeyDown( KeyCode::Left  ) ) CameraRotation.y += RotationSpeed;
		if ( Input::IsKeyDown( KeyCode::Right ) ) CameraRotation.y -= RotationSpeed;
		if ( Input::IsKeyDown( KeyCode::Up    ) ) CameraRotation.x += RotationSpeed;
		if ( Input::IsKeyDown( KeyCode::Down  ) ) CameraRotation.x -= RotationSpeed;

		CameraRotation.x = Math::Clamp( CameraRotation.x, -Math::Radians( 89.9f ), Math::Radians( 89.9f ) );

		CameraTransform->SetLocalRotation( Quaternion::ToQuaternion( Vector3( CameraRotation, 0.0f ) ) );

		if ( Input::IsKeyDown( KeyCode::A ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalLeft()     * MovementSpeed );
		if ( Input::IsKeyDown( KeyCode::D ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalRight()    * MovementSpeed );
		if ( Input::IsKeyDown( KeyCode::W ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalForward()  * MovementSpeed );
		if ( Input::IsKeyDown( KeyCode::S ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalBackward() * MovementSpeed );
		if ( Input::IsKeyDown( KeyCode::Q ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalDown()     * MovementSpeed );
		if ( Input::IsKeyDown( KeyCode::E ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalUp()       * MovementSpeed );
	}

private:

	Vector2 CameraRotation = Vector2::Zero;
};