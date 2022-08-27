#include "CGE.hpp"
#include "Component.hpp"
#include "GameObject.hpp"
#include "Prefab.hpp"
#include "CameraController.hpp"

void InitializeScene()
{
	auto prefab = Resource::Load< Prefab >( "spear"_H );
	GameObject object = Prefab::Instantiate( *prefab );
	MeshRenderer* renderer = object.GetComponentInChild< MeshRenderer >();
	renderer->GetMaterial()->SetShader( Shader::Diffuse );
	object.GetTransform()->SetGlobalScale( Vector3::One );
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
}

int main()
{
	auto Window = ConsoleWindow::Create( "Title goes here.", { 64, 64 }, { 16, 16 } );
	ConsoleWindow::MakeContextCurrent( Window );
	CGE::Init();

	InitializeScene();

	
	GameObject CameraObject = *Camera::GetMainCamera()->GetOwner();
	CameraObject.GetTransform()->SetLocalPosition( Vector3( 0.0f, 2.0f, -3.0f ) );
	CameraObject.AddComponent< CameraController >();

	Action<> GameLoop = [&]()
	{
		CameraObject.GetComponent< CameraController >()->Update();
	};

	CGE::Run( GameLoop );

}