#pragma once
#include "Renderer.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Transform.hpp"
#include "Material.hpp"
#include "Rendering.hpp"

// Remove later
#include "Camera.hpp"
#include "ScreenBuffer.hpp"
#include "Light.hpp"
#include "File.hpp"
#include "Rect.hpp"
#include "Frustum.hpp"

template < typename T >
class IMeshRenderer;

typedef IMeshRenderer< void > MeshRenderer;

template < typename T >
class IMeshRenderer : public IRenderer< IMeshRenderer< T > >
{
public:

	void OnDraw( RenderQueue& a_Queue ) const override
	{
		if ( !Camera::GetMainCamera() || !m_Mesh || !m_Material )
		{
			return;
		}

		Frustum ViewFrustum = *Camera::GetMainCamera();
		float Radius = m_Mesh->GetRadius();
		bool Collided = false;

		// Perform broadphase frustum culling.
		for ( const auto& FrustumPlane : ViewFrustum.Planes )
		{
			if ( Geometry::DistanceFromPlane( FrustumPlane, this->GetOwner().GetTransform()->GetGlobalPosition() ) > -Radius )
			{
				Collided = true;
				break;
			}
		}

		if ( !Collided )
		{
			return;
		}

		RenderInstruction Instruction;
		Instruction.Modification = RenderInstruction::Modification::SET;
		Instruction.Object = RenderInstruction::Object::Mesh;
		Instruction.Source = m_Mesh;
		a_Queue += Instruction;

		Instruction.Modification = RenderInstruction::Modification::SET;
		Instruction.Object = RenderInstruction::Object::Material;
		Instruction.Source = m_Material;
		a_Queue += Instruction;

		Instruction.Modification = RenderInstruction::Modification::SET;
		Instruction.Object = RenderInstruction::Object::Model;
		Instruction.Source = &this->GetOwner().GetTransform()->GetGlobalMatrix();
		a_Queue += Instruction;

		Instruction.Modification = RenderInstruction::Modification::DRAW;
		Instruction.Object = RenderInstruction::Object::None;
		Instruction.Source = nullptr;
		a_Queue += Instruction;
	}

	void SetMesh( const Mesh* a_Mesh )
	{
		m_Mesh = a_Mesh;
	}

	void SetMaterial( const Material* a_Material )
	{
		m_Material = a_Material;
	}

	void SetRenderMode( RenderMode a_RenderMode )
	{
		m_RenderMode = a_RenderMode;
	}

private:

	const Mesh*       m_Mesh;
	const Material*   m_Material;
	RenderMode        m_RenderMode; // unused at the moment.
};