#include "Texture.hpp"
#include "Mesh.hpp"
#include "Input.hpp"
#include "Light.hpp"
#include "MeshRenderer.hpp"
#include "Transform.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "File.hpp"
#include "Resource.hpp"
#include "ConsoleWindow.hpp"
#include "Rendering.hpp"
#include "Math.hpp"
#include "CGE.hpp"
#include "Prefab.hpp"

#include <iostream>
int main()
{
	auto Window = ConsoleWindow::Create( "Title goes here.", { 64, 64 }, { 8, 8 } );
	ConsoleWindow::MakeContextCurrent( Window );
	CGE::Init();

	/*auto grass_prefab = Resource::Load< Prefab >( "spear"_H );
	GameObject grass_object = Prefab::Instantiate( *grass_prefab );
	MeshRenderer* mesh_renderer = grass_object.GetComponentInChild< MeshRenderer >();
	auto mat = mesh_renderer->GetMaterial().Assure();
	auto mesh = mesh_renderer->GetMesh().Assure();
	mat->SetShader( Shader::Diffuse );
	grass_object.GetTransform()->SetGlobalPositionY( -2.0f );*/

	auto prefab = Resource::Load< Prefab >( "spear"_H );
	//auto object = Prefab::Instantiate( *prefab );
	//auto textre = Resource::Load< Texture2D >( "landscape"_H );
	//MeshRenderer* renderer = object.GetComponentInChild< MeshRenderer >();
	//auto mater = renderer->GetMaterial();
	//auto mesh = renderer->GetMesh();

	GameObject object = GameObject::Instantiate();
	MeshRenderer* renderer = object.AddComponent< MeshRenderer >();
	auto mesh = Resource::Load< Mesh >( "spear_mesh0"_H );
	auto material = Resource::Load< Material >( "spear_material0"_H );
	renderer->SetMesh( mesh );
	renderer->SetMaterial( material );
	//mater->AddTexture( "texture_diffuse"_N, textre );
	material.Assure()->SetShader( Shader::Diffuse );
	//object.GetTransform()->SetGlobalRotation( Quaternion::ToQuaternion( Vector3{ 0.25f, 0.0f, 0.0f } ) );
	object.GetTransform()->SetGlobalScale( Vector3{ 0.4f, 0.4f, 0.4f } );

	GameObject CameraObject = GameObject::Instantiate( "Camera"_N );
	Camera* CameraComponent = CameraObject.AddComponent< Camera >();
	//CameraObject.GetTransform()->SetGlobalRotation( Quaternion::ToQuaternion( Vector3::Zero ) );
	CameraComponent->SetAspect( 1.0f );
	CameraComponent->SetFOV( Math::Radians( 90.0f ) );
	CameraComponent->SetNearZ( 0.1f );
	CameraComponent->SetFarZ( 100.0f );
	Camera::SetMainCamera( CameraComponent );
	auto renderers = Component::GetComponents< Renderer >();

	Vector2 CameraRotation = Vector2::Zero;
	Vector3 CameraPosition = Vector3( 0.0f, 0.0f, -4.0f );

	Action<> GameLoop = [&]()
	{
		Sleep( 1 );

		CameraObject.GetTransform()->RotateGlobal( Vector3( .0f, .0f, .01f ) );
		CameraRotation += Input::GetMouseDelta() * 0.1f;
		CameraObject.GetTransform()->SetGlobalRotation( Quaternion::ToQuaternion( Vector3{ CameraRotation.y, -CameraRotation.x, 0.0f }, RotationOrder::ZXY ) );

		if ( Input::IsKeyDown( KeyCode::A ) ) CameraPosition.x -= 0.1f;
		if ( Input::IsKeyDown( KeyCode::D ) ) CameraPosition.x += 0.1f;
		if ( Input::IsKeyDown( KeyCode::W ) ) CameraPosition.z += 0.1f;
		if ( Input::IsKeyDown( KeyCode::S ) ) CameraPosition.z -= 0.1f;
		if ( Input::IsKeyDown( KeyCode::Q ) ) CameraPosition.y -= 0.1f;
		if ( Input::IsKeyDown( KeyCode::E ) ) CameraPosition.y += 0.1f;

		CameraObject.GetTransform()->SetGlobalPosition( CameraPosition );
	};

	CGE::Run( GameLoop );
	
	return 0;
}