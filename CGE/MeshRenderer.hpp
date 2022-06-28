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

DefineComponent( MeshRenderer, Renderer )
{
public:
	
	void OnRender( RenderQueue& a_Queue ) const override
	{
		if ( !Camera::GetMainCamera() )
		{
			return;
		}

		if ( !m_Mesh.Assure() || !m_Material.Assure() )
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
		Instruction.Source = m_Mesh.Assure();
		a_Queue += Instruction;

		Instruction.Modification = RenderInstruction::Modification::SET;
		Instruction.Object = RenderInstruction::Object::Material;
		Instruction.Source = m_Material.Assure();
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

	void SetMesh( ResourceHandle< Mesh > a_Mesh )
	{
		m_Mesh = a_Mesh;
	}

	void SetMaterial( ResourceHandle< Material > a_Material )
	{
		m_Material = a_Material;
	}

private:

	friend class ResourcePackager;
	friend class Serialization;

	template < typename _Serializer >
	void Serialize( _Serializer& a_Serializer ) const
	{
		a_Serializer << m_Mesh << m_Material;
	}

	template < typename _Deserializer >
	void Deserialize( _Deserializer& a_Deserializer )
	{
		a_Deserializer >> m_Mesh >> m_Material;
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer & m_Mesh & m_Material;
	}

	ResourceHandle< Mesh     > m_Mesh;
	ResourceHandle< Material > m_Material;
};