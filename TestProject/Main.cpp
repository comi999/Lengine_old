#include "CGE.hpp"
#include "Component.hpp"
#include "GameObject.hpp"
#include "Prefab.hpp"
#include "AudioClip.hpp"
#include "AudioSource.hpp"
#include "AudioListener.hpp"

int main()
{
	auto Window = ConsoleWindow::Create( "Title goes here.", { 64, 64 }, { 16, 16 } );
	ConsoleWindow::MakeContextCurrent( Window );
	CGE::Init();

	auto prefab = Resource::Load< Prefab >( "spear"_H );
	//auto audioClip = Resource::Load< AudioClip >( "sound"_H );
	auto audioClip = Resource::Load< SfxrClip >( "door"_H );


	for ( int i = 0; i < 1; ++i )
	{
		GameObject object = Prefab::Instantiate( *prefab );

		auto audioSource = object.AddComponent<AudioSource>();
		//audioSource->LoadWav(audioClip);
		audioSource->LoadSfx(audioClip);
		audioSource->Play();
		audioSource->SetLooping(true);

		MeshRenderer* renderer = object.GetComponentInChild< MeshRenderer >();
		renderer->GetMaterial()->SetShader( Shader::Diffuse );
		object.GetTransform()->SetGlobalScale( Vector3::One );
	}
	GameObject SunObject = GameObject::Instantiate( "Sun"_N );
	Light* LightComponent = SunObject.AddComponent< Light >();
	Light::SetSun( LightComponent );

	GameObject CameraObject = GameObject::Instantiate( "Camera"_N );
	Camera* CameraComponent = CameraObject.AddComponent< Camera >();
	CameraObject.AddComponent<AudioListener>();
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

		if ( Input::IsKeyDown( KeyCode::A ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalLeft()     * 1.1f );
		if ( Input::IsKeyDown( KeyCode::D ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalRight()    * 1.1f );
		if ( Input::IsKeyDown( KeyCode::W ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalForward()  * 1.1f );
		if ( Input::IsKeyDown( KeyCode::S ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalBackward() * 1.1f );
		if ( Input::IsKeyDown( KeyCode::Q ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalDown()     * 1.1f );
		if ( Input::IsKeyDown( KeyCode::E ) ) CameraTransform->TranslateLocal( CameraTransform->GetLocalUp()       * 1.1f );
	};

	CGE::Run( GameLoop );

}