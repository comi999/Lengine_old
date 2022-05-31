#pragma once
#include "ECS.hpp"
#include "RenderBuffer.hpp"
#include "RenderState.hpp"
#include "RenderStage.hpp"
#include "RenderInstruction.hpp"
#include "ShaderPipeline.hpp"
#include "DrawCall.hpp"
#include "Frustum.hpp"
#include "MeshRenderer.hpp"

class RenderPipeline
{
public:

	void Begin()
	{
		m_RenderStage = RenderStage::COLLATE_DRAW_CALLS;

		while ( m_RenderStage != RenderStage::END )
		{
			ProcessStage( m_RenderStage );
		}
	}

	inline ShaderPipeline& GetShaderPipeline()
	{
		return m_ShaderPipeline;
	}

private:

	void ProcessStage( RenderStage a_RenderStage )
	{
		/*switch ( a_RenderStage )
		{
			case RenderStage::COLLATE_DRAW_CALLS:
			{
				Stage_CollateDrawCalls( *this );
				m_RenderStage = RenderStage::POPULATE_INSTRUCTION_QUEUE;
				break;
			}
			case RenderStage::POPULATE_INSTRUCTION_QUEUE:
			{
				Stage_PopulateQueue( *this );
				m_RenderStage = RenderStage::PROCESS_INSTRUCTION_QUEUE;
				break;
			}
			case RenderStage::PROCESS_INSTRUCTION_QUEUE:
			{
				Stage_ProcessInstructionQueue( *this );
				m_RenderStage = RenderStage::END;
				break;
			}
			case RenderStage::END:
			{
				break;
			}
		}
	*/}

	static void Stage_CollateDrawCalls( RenderPipeline& a_RenderingPipeline )
	{
	//	// Needs to be all renderers.
	//	auto AllComponents = ECS::GetAllExactComponents< MeshRenderer >();
	//	a_RenderingPipeline.m_DrawCalls.reserve( AllComponents.size() );
	//	Frustum CameraFrustum = Frustum::GenerateFrustum( *Camera::GetMainCamera() );

	//	auto Check = []( const DrawCall& a_DrawCall, const Frustum& a_Frustum )
	//	{
	//		return false;
	//	};

	//	for ( auto Begin = AllComponents.begin(), End = AllComponents.end(); Begin != End; ++Begin )
	//	{
	//		DrawCall Call = Begin->GenerateDrawCall();

	//		if ( Check( Call, CameraFrustum ) )
	//		{
	//			a_RenderingPipeline.m_DrawCalls.push_back( Call );
	//		}
	//	}
	}

	static void Stage_PopulateQueue( RenderPipeline& a_RenderingPipeline )
	{
		/*std::sort( a_RenderingPipeline.m_DrawCalls.begin(), a_RenderingPipeline.m_DrawCalls.end() );
		RenderInstruction MeshInstruction;
		RenderInstruction MaterialInstruction;
		RenderInstruction ModelInstruction;
		RenderInstruction ModeInstruction;
		DrawCall CurrentCall;

		for ( auto Begin = a_RenderingPipeline.m_DrawCalls.begin(), End = a_RenderingPipeline.m_DrawCalls.end(); Begin != End; ++Begin )
		{
			CurrentCall = *Begin;
			MeshInstruction.Modification = RenderInstruction::Modification::SET;
			MeshInstruction.Object = RenderObject::Mesh;
			MeshInstruction.Source = Begin->ActiveMesh;
			a_RenderingPipeline.m_RenderQueue.push_back( MeshInstruction );

			for ( ; Begin != End && Begin->ActiveMesh == MeshInstruction.Source; ++Begin )
			{
				MaterialInstruction.Modification = RenderInstruction::Modification::SET;
				MaterialInstruction.Object = RenderObject::Material;
				MaterialInstruction.Source = Begin->ActiveMaterial;
				a_RenderingPipeline.m_RenderQueue.push_back( MaterialInstruction );

				for ( ; Begin != End && Begin->ActiveMaterial == MaterialInstruction.Source; ++Begin )
				{
					ModelInstruction.Modification = RenderInstruction::Modification::SET;
					ModelInstruction.Object = RenderObject::Model;
					ModelInstruction.Source = Begin->ActiveModel;
					a_RenderingPipeline.m_RenderQueue.push_back( ModelInstruction );

					for ( ; Begin != End && Begin->ActiveModel == ModelInstruction.Source; ++Begin )
					{
						ModeInstruction.Modification = RenderInstruction::Modification::SET;
						ModeInstruction.Object = RenderObject::RenderMode;
						ModeInstruction.Index = static_cast< uint32_t >( Begin->ActiveRenderMode );
						a_RenderingPipeline.m_RenderQueue.push_back( ModeInstruction );

						for ( ; Begin != End && Begin->ActiveRenderMode == static_cast< RenderMode >( ModeInstruction.Index ); ++Begin )
						{
							a_RenderingPipeline.m_RenderQueue.emplace_back( RenderInstruction::Modification::DRAW, nullptr, nullptr, 0 );
						}
					}
				}
			}
		}
	*/}

	static void Stage_ProcessInstructionQueue( RenderPipeline& a_RenderingPipeline )
	{
		//for ( auto Begin = a_RenderingPipeline.m_RenderQueue.begin(), End = a_RenderingPipeline.m_RenderQueue.end(); Begin != End; ++Begin )
		//{
		//	RenderInstruction Instruction = *Begin;

		//	switch ( Instruction.Modification )
		//	{
		//		case RenderInstruction::Modification::SET:
		//		{
		//			switch ( Instruction.Object )
		//			{
		//				case RenderObject::Mesh:
		//				{
		//					const Mesh* ActiveMesh = ( const Mesh* )Instruction.Source;
		//					a_RenderingPipeline.m_RenderState.ActiveMesh = ActiveMesh;

		//					size_t VAOStride = 
		//						sizeof( size_t ) * a_RenderingPipeline.m_RenderState.PositionInterpolationActive +
		//						sizeof( size_t ) * a_RenderingPipeline.m_RenderState.ColourInterpolationActive +
		//						sizeof( size_t ) * a_RenderingPipeline.m_RenderState.NormalInterpolationActive + 
		//						sizeof( size_t ) * a_RenderingPipeline.m_RenderState.TexelInterpolationActive;

		//					size_t VBOStride =
		//						sizeof( Vector4 ) * a_RenderingPipeline.m_RenderState.PositionInterpolationActive +
		//						sizeof( Vector4 ) * a_RenderingPipeline.m_RenderState.ColourInterpolationActive +
		//						sizeof( Vector3 ) * a_RenderingPipeline.m_RenderState.NormalInterpolationActive +
		//						sizeof( Vector2 ) * a_RenderingPipeline.m_RenderState.TexelInterpolationActive;

		//					// Setup the vao
		//					a_RenderingPipeline.m_VAOBuffer.Reset( ActiveMesh->GetVertexCount() / 3,  );


		//					break;
		//				}
		//				case RenderObject::Material:
		//				{
		//					a_RenderingPipeline.m_RenderState.ActiveMaterial = ( Material* )Instruction.Source;
		//					break;
		//				}
		//				case RenderObject::Model:
		//				{
		//					break;
		//				}
		//				case RenderObject::RenderMode:
		//				{
		//					break;
		//				}
		//			}
		//			break;
		//		}
		//		case RenderInstruction::Modification::ENABLE:
		//		{
		//			break;
		//		}
		//		case RenderInstruction::Modification::DISABLE:
		//		{
		//			break;
		//		}
		//		case RenderInstruction::Modification::DRAW:
		//		{
		//			a_RenderingPipeline.m_ShaderPipeline.Begin();
		//			break;
		//		}
		//	}
		//}
	}

	RenderStage                      m_RenderStage;
	RenderState                      m_RenderState;
	ShaderPipeline                   m_ShaderPipeline;
	std::vector< RenderBuffer >      m_RenderBuffers;
	std::vector< DrawCall >          m_DrawCalls;
	std::vector< RenderInstruction > m_RenderQueue;
	std::vector< RenderBuffer >      m_VertexArrayObjects;
	std::vector< RenderBuffer >      m_VertexBufferObjects;
};