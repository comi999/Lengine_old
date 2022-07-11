#include "CGE.hpp"
#include "Component.hpp"
#include "GameObject.hpp"
#include "Prefab.hpp"

int main()
{
	auto Window = ConsoleWindow::Create( "Title goes here.", { 64, 64 }, { 16, 16 } );
	ConsoleWindow::MakeContextCurrent( Window );
	CGE::Init();

	auto prefab = Resource::Load< Prefab >( "spear"_H );

	Material RedMaterial = Material::LitFlatColour;

	RedMaterial.SetProperty( "diffuse_colour"_H, ( Vector4 )Colour::RED );

	for ( int i = 0; i < 1; ++i )
	{

		GameObject object = Prefab::Instantiate( *prefab );

		MeshRenderer* renderer = object.GetComponentInChild< MeshRenderer >();
		//renederer->GetMaterial().Assure()->SetShader( Shader::Diffuse );
		renderer->SetMaterial( RedMaterial );
		object.GetTransform()->SetGlobalScale( Vector3::One );
	}

	GameObject SunObject = GameObject::Instantiate( "Sun"_N );
	Light* LightComponent = SunObject.AddComponent< Light >();
	Light::SetSun( LightComponent );

	GameObject CameraObject = GameObject::Instantiate( "Camera"_N );
	Camera* CameraComponent = CameraObject.AddComponent< Camera >();
	CameraComponent->SetAspect( 1.0f );
	CameraComponent->SetFOV( Math::Radians( 75.0f ) );
	CameraComponent->SetNearZ( 0.1f );
	CameraComponent->SetFarZ( 100.0f );
	Camera::SetMainCamera( CameraComponent );
	auto renderers = Component::GetComponents< Renderer >();

	Vector2 CameraRotation = Vector2::Zero;
	CameraObject.GetTransform()->SetLocalPosition( Vector3( 0.0f, 2.0f, -3.0f ) );

	float i = 0.0f;

	Action<> GameLoop = [&]()
	{
		Transform* CameraTransform = CameraObject.GetTransform();

		if ( Input::IsKeyDown( KeyCode::Left  ) ) CameraRotation.y += 0.1f;
		if ( Input::IsKeyDown( KeyCode::Right ) ) CameraRotation.y -= 0.1f;
		if ( Input::IsKeyDown( KeyCode::Up    ) ) CameraRotation.x += 0.1f;
		if ( Input::IsKeyDown( KeyCode::Down  ) ) CameraRotation.x -= 0.1f;

		CameraRotation.x = Math::Clamp( CameraRotation.x, -Math::Radians( 89.9f ), Math::Radians( 89.9f ) );

		CameraTransform->SetLocalRotation( Quaternion::ToQuaternion( Vector3( CameraRotation, 0.0f ) ) );

		if ( Input::IsKeyDown( KeyCode::A ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalLeft()     * 0.1f );
		if ( Input::IsKeyDown( KeyCode::D ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalRight()    * 0.1f );
		if ( Input::IsKeyDown( KeyCode::W ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalForward()  * 0.1f );
		if ( Input::IsKeyDown( KeyCode::S ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalBackward() * 0.1f );
		if ( Input::IsKeyDown( KeyCode::Q ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalDown()     * 0.1f );
		if ( Input::IsKeyDown( KeyCode::E ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalUp()       * 0.1f );
	};

	CGE::Run( GameLoop );

}