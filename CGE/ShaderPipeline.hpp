#pragma once
#include "ShaderStage.hpp"

//class ShaderPipeline
//{
//private:
//
//	using StageProcessor = void( * )( ShaderPipeline& );
//	using VertexShader = void( * )( void );
//	using TesselationShader = void( * )( void );
//	using GeometryShader = void( * )( void );
//	using FragmentShader = void( * )( void );
//
//public:
//
//	void Begin()
//	{
//		m_ShaderStage = ShaderStage::VERTEX_SPECIFICATION;
//
//		while ( m_ShaderStage != ShaderStage::END )
//		{
//			ProcessStage( m_ShaderStage );
//		}
//	}
//
//	inline void SetVertexShader( VertexShader a_VertexShader )
//	{
//		m_VertexShader = a_VertexShader;
//	}
//
//	inline void SetTesselationShader( TesselationShader a_TesselationShader )
//	{
//		m_TesselationShader = a_TesselationShader;
//	}
//
//	inline void SetGeometryShader( GeometryShader a_GeometryShader )
//	{
//		m_GeometryShader = a_GeometryShader;
//	}
//
//	inline void SetFragmentShader( FragmentShader a_FragmentShader )
//	{
//		m_FragmentShader = a_FragmentShader;
//	}
//
//private:
//
//	void ProcessStage( ShaderStage a_ShaderStage )
//	{
//		switch ( a_ShaderStage )
//		{
//			case ShaderStage::VERTEX_SPECIFICATION:
//			{
//				Stage_VertexSpecification( *this );
//				m_ShaderStage = ShaderStage::VERTEX_SHADER;
//				break;
//			}
//			case ShaderStage::VERTEX_SHADER:
//			{
//				Stage_VertexShader( *this );
//				m_ShaderStage = ShaderStage::TESELLATION_SHADER;
//				break;
//			}
//			case ShaderStage::TESELLATION_SHADER:
//			{
//				Stage_TesselationShader( *this );
//				m_ShaderStage = ShaderStage::GEOMETRY_SHADER;
//				break;
//			}
//			case ShaderStage::GEOMETRY_SHADER:
//			{
//				Stage_GeometryShader( *this );
//				m_ShaderStage = ShaderStage::VERTEX_POST_PROCESSING;
//				break;
//			}
//			case ShaderStage::VERTEX_POST_PROCESSING:
//			{
//				Stage_VertexPostProcessing( *this );
//				m_ShaderStage = ShaderStage::PRIMITIVE_ASSEMBLY;
//				break;
//			}
//			case ShaderStage::PRIMITIVE_ASSEMBLY:
//			{
//				Stage_PrimitiveAssembly( *this );
//				m_ShaderStage = ShaderStage::RASTERIZATION;
//				break;
//			}
//			case ShaderStage::RASTERIZATION:
//			{
//				Stage_Rasterization( *this );
//				m_ShaderStage = ShaderStage::FRAGMENT_SHADER;
//				break;
//			}
//			case ShaderStage::FRAGMENT_SHADER:
//			{
//				Stage_FragmentShader( *this );
//				m_ShaderStage = ShaderStage::PER_SAMPLE_OPERATIONS;
//				break;
//			}
//			case ShaderStage::PER_SAMPLE_OPERATIONS:
//			{
//				Stage_PerSampleOperations( *this );
//				m_ShaderStage = ShaderStage::END;
//				break;
//			}
//			case ShaderStage::END:
//			{
//				break;
//			}
//		}
//	}
//
//	static void Stage_VertexSpecification( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	static void Stage_VertexShader( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	static void Stage_TesselationShader( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	static void Stage_GeometryShader( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	static void Stage_VertexPostProcessing( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	static void Stage_PrimitiveAssembly( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	static void Stage_Rasterization( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	static void Stage_FragmentShader( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	static void Stage_PerSampleOperations( ShaderPipeline& a_ShaderPipeline )
//	{
//
//	}
//
//	ShaderStage       m_ShaderStage;
//	VertexShader      m_VertexShader;
//	TesselationShader m_TesselationShader;
//	GeometryShader    m_GeometryShader;
//	FragmentShader    m_FragmentShader;
//};