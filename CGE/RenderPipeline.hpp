#pragma once
#include <list>
#include <set>
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

class RenderPipeline
{
private:

	friend class CGE;

	static void Tick()
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
		Rendering::Clear( ( uint8_t )BufferFlag::COLOUR_BUFFER_BIT );
		bool DepthTestActive;
		Rendering::GetBooleanv( RenderSetting::DEPTH_TEST, &DepthTestActive );
		Rendering::Enable( RenderSetting::CULL_FACE );
		Rendering::CullFace( CullFaceMode::BACK );
		Rendering::Enable( RenderSetting::DEPTH_TEST );

		if ( DepthTestActive )
		{
			Rendering::ClearDepth( 1000.0f );
			Rendering::Clear( ( uint8_t )BufferFlag::DEPTH_BUFFER_BIT );
		}

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
							s_ActiveMesh = static_cast< const Mesh* >( Instruction.ResourceSource );
							break;
						}
						case RenderInstruction::Object::Material:
						{
							s_ActiveMaterial = static_cast< const Material* >( Instruction.ResourceSource );
							break;
						}
						case RenderInstruction::Object::Model:
						{
							s_ActiveModel = static_cast< const Matrix4* >( Instruction.ResourceSource );
							break;
						}
					}

					s_Dirty = true;
					break;
				}
				case RenderInstruction::Modification::DRAW:
				{
					Draw();
				}
			}

			Queue.Pop();
		}

	}

	static void ApplyAssets()
	{
		// Apply mesh attributes.
		if ( s_ActiveMesh )
		{
			if ( !s_ArrayHandle )
			{
				Rendering::GenVertexArrays( 1, &s_ArrayHandle );
			}

			for ( uint32_t i = 0; i < 6; ++i )
			{
				if ( !s_BufferHandles[ i ] )
				{
					Rendering::GenBuffers( 1, &s_BufferHandles[ i ] );
				}
			}

			Rendering::BindVertexArray( s_ArrayHandle );
			uint32_t VertexCount = s_ActiveMesh->GetVertexCount();

			if ( s_ActiveMesh->HasPositions() )
			{
				Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, s_BufferHandles[ 0 ] );
				Rendering::BufferData( BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), s_ActiveMesh->GetPositions(), DataUsage::DRAW );
				Rendering::VertexAttribPointer( 0, 3, DataType::FLOAT, false, sizeof( Vector3 ), ( void* )0 );
				Rendering::EnableVertexAttribArray( 0 );
			}

			if ( s_ActiveMesh->HasTexels() )
			{
				Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, s_BufferHandles[ 1 ] );
				Rendering::BufferData( BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector2 ), s_ActiveMesh->GetTexels(), DataUsage::DRAW );
				Rendering::VertexAttribPointer( 1, 2, DataType::FLOAT, false, sizeof( Vector2 ), ( void* )0 );
				Rendering::EnableVertexAttribArray( 1 );
			}

			if ( s_ActiveMesh->HasColours() )
			{
				Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, s_BufferHandles[ 2 ] );
				Rendering::BufferData( BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector4 ), s_ActiveMesh->GetColours(), DataUsage::DRAW );
				Rendering::VertexAttribPointer( 2, 4, DataType::FLOAT, false, sizeof( Vector4 ), ( void* )0 );
				Rendering::EnableVertexAttribArray( 2 );
			}

			if ( s_ActiveMesh->HasNormals() )
			{
				Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, s_BufferHandles[ 3 ] );
				Rendering::BufferData( BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), s_ActiveMesh->GetNormals(), DataUsage::DRAW );
				Rendering::VertexAttribPointer( 3, 3, DataType::FLOAT, false, sizeof( Vector3 ), ( void* )0 );
				Rendering::EnableVertexAttribArray( 3 );
			}

			if ( s_ActiveMesh->HasTangents() )
			{
				Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, s_BufferHandles[ 4 ] );
				Rendering::BufferData( BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), s_ActiveMesh->GetTangents(), DataUsage::DRAW );
				Rendering::VertexAttribPointer( 4, 3, DataType::FLOAT, false, sizeof( Vector3 ), ( void* )0 );
				Rendering::EnableVertexAttribArray( 4 );
			}

			if ( s_ActiveMesh->HasBitangents() )
			{
				Rendering::BindBuffer( BufferTarget::ARRAY_BUFFER, s_BufferHandles[ 5 ] );
				Rendering::BufferData( BufferTarget::ARRAY_BUFFER, VertexCount * sizeof( Vector3 ), s_ActiveMesh->GetBitangents(), DataUsage::DRAW );
				Rendering::VertexAttribPointer( 5, 3, DataType::FLOAT, false, sizeof( Vector3 ), ( void* )0 );
				Rendering::EnableVertexAttribArray( 5 );
			}

			Rendering::BindVertexArray( 0 );
			Rendering::BindVertexArray( s_ArrayHandle );
		}
		
		// Apply material properties.
		if ( s_ActiveMaterial )
		{
			s_ActiveMaterial->Apply();
		}
	}

	static void Draw()
	{
		if ( s_Dirty )
		{
			ApplyAssets();
			s_Dirty = false;
		}

		// Set PVM
		if ( s_ActiveModel )
		{
			static Matrix4 PV;
			PV = Camera::GetMainCamera()->GetProjectionViewMatrix();

			static Matrix4 PVM;
			PVM = Math::Multiply( PV, *s_ActiveModel );

			int32_t PVMLocation = Rendering::GetUniformLocation( s_ActiveMaterial->GetShader().GetProgramHandle(), "u_PVM" );

			if ( PVMLocation >= 0 )
			{
				Rendering::UniformMatrix4fv( PVMLocation, 1, false, &PVM[ 0 ] );
			}

			int32_t ModelLocation = Rendering::GetUniformLocation( s_ActiveMaterial->GetShader().GetProgramHandle(), "u_Model" );

			if ( ModelLocation >= 0 )
			{
				Rendering::UniformMatrix4fv( ModelLocation, 1, false, s_ActiveModel->Data );
			}
		}

		// Set Sun0 Direction
		if ( int32_t Sun0DirectionLocation = Rendering::GetUniformLocation( s_ActiveMaterial->GetShader().GetProgramHandle(), "u_SunDirection0" ); Sun0DirectionLocation >= 0 )
		{
			Vector3 SunDirection;

			if ( const Light* Sun = Light::GetSun() )
			{
				SunDirection = Sun->GetDirection();
			}
			else
			{
				SunDirection = Vector3::Zero;
			}

			Rendering::Uniform3f( Sun0DirectionLocation, SunDirection.x, SunDirection.y, SunDirection.z );
		}

		// Set Sun0 Ambient
		if ( int32_t Sun0AmbientLocation = Rendering::GetUniformLocation( s_ActiveMaterial->GetShader().GetProgramHandle(), "u_SunAmbient0" ); Sun0AmbientLocation >= 0 )
		{
			Vector3 SunAmbient;

			if ( const Light* Sun = Light::GetSun() )
			{
				SunAmbient = Sun->GetAmbient();
			}
			else
			{
				SunAmbient = Vector3::Zero;
			}

			Rendering::Uniform3f( Sun0AmbientLocation, SunAmbient.x, SunAmbient.y, SunAmbient.z );
		}

		

		// Draw code.
		if ( s_ActiveMesh )
		{
			Rendering::DrawElements( RenderMode::TRIANGLE, s_ActiveMesh->GetIndexCount(), DataType::UNSIGNED_INT, s_ActiveMesh->GetIndices() );
		}
	}
	
	inline static bool            s_Dirty;
	inline static const Mesh*     s_ActiveMesh;
	inline static const Material* s_ActiveMaterial;
	inline static const Matrix4*  s_ActiveModel;
	inline static const Matrix4*  s_ActivePV;
	inline static ArrayHandle     s_ArrayHandle;
	inline static BufferHandle    s_BufferHandles[ 6 ];
};