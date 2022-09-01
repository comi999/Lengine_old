#include <list>
#include <set>

#include "RenderingPipeline.hpp"
#include "Component.hpp"
#include "Mesh.hpp"
#include "Frustum.hpp"
#include "Renderer.hpp"
#include "Rendering.hpp"
#include "RenderInstruction.hpp"
#include "RenderQueue.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "MeshRenderer.hpp"

void RenderingPipeline::Init()
{
	Rendering::Init();
}

void RenderingPipeline::Tick()
{
	// Collect all instructions.
	auto Renderers = Component::GetComponents< Renderer >();
	RenderQueue Queue;

	for ( auto Renderer : Renderers )
	{
		Renderer->OnRender( Queue );
	}

	// Sort all render instructions
	//s_RenderInstructions.sort();

	// Remove this later
	//Sleep( 33 );

	Rendering::Clear();

	// Process queue.
	while ( !Queue.Empty() )
	{
		auto& Instruction = Queue.Front();

		switch ( Instruction.Modification )
		{
			case RenderInstruction::Modification::SET:
			{
				switch ( Instruction.Object )
				{
					case RenderInstruction::Object::Mesh:
					{
						Rendering::ApplyMesh( *static_cast< const Mesh* >( Instruction.ResourceSource ) );
						break;
					}
					case RenderInstruction::Object::Material:
					{
						Rendering::ApplyMaterial( *static_cast< const Material* >( Instruction.ResourceSource ) );
						break;
					}
					case RenderInstruction::Object::Model:
					{
						const Matrix4* ActiveModel = static_cast< const Matrix4* >( Instruction.ResourceSource );
						Rendering::ApplyUniform( "u_Model", 16, &( *ActiveModel )[ 0 ] );
						Matrix4 PVM = Math::Multiply( Camera::GetMainCamera()->GetProjectionViewMatrix(), *ActiveModel );
						Rendering::ApplyUniform( "u_PVM", 16, &PVM );
						break;
					}
				}

				s_Dirty = true;
				break;
			}
			case RenderInstruction::Modification::DRAW:
			{
				Rendering::Draw();
			}
		}

		Queue.Pop();
	}
}

void RenderingPipeline::Draw()
{
	Vector3 SunDirection;
	Vector3 SunAmbient;

	if ( const Light* Sun = Light::GetSun() )
	{
		SunDirection = Sun->GetDirection();
		SunAmbient = Sun->GetAmbient();
	}
	else
	{
		SunDirection = Vector3::Zero;
		SunAmbient = Vector3::Zero;
	}

	Rendering::ApplyUniform( "u_SunDirection0", 3, &SunDirection[ 0 ] );
	Rendering::ApplyUniform( "u_SunAmbient0", 3, &SunAmbient[ 0 ] );	

	Rendering::Draw();
}