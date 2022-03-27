#pragma once
#include <vector>
#include <bitset>
#include "entt/entt.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

// broad phase filtering: remove OBJECTS that will definitely not show up on screen by using encompassing regions and frustum planes
// vertex shader transform vertices into clip space
// everything is now in clip space
// clipping
// move from clip space to ndc space / could be perspective correct/affine/flat
// ndc to screen space
// fragment shader when rasterizing triangle on screen

// MESH:
// Contains:
// - Array of TriangleIndices
// - Array of VertixIndices
// - Array of VertexColours
// - Array of VertexPositions
// - Array of VertexNormals
// - Array of VertexTangents
// - Array of VertexBitangents
// - Array of VertexTexels

// MATERIAL:
// Contains:
// - Shader reference
// - Texture list
// - Can contain solid colour
// - Rendering settings (like ambient colour, etc)

// DRAW CALL:
// A draw call consists of:
// - Mesh reference
// - Material reference
// - Primitive render type: (Point, Line, Triangle)
// - ModelMatrix

// PROCESS:
// - All render components are called and draw calls are created from each one.
// - A draw call is rejected if model exists outside of frustum.
// - Draw calls are inserted based on sorting criteria.
// - Draw queue is processed and render commands are generated.
// - Render commands are sent to rendering thread and processed.
// - Post processing. Allow to render to target texture and use it for post processing.

// SHADER:
// - Vertex shader iterates over input vertex buffer and processes all of them converting them into clip space and copying into output vertex buffer
// - Based on primitive rendering mode (point/line/triangle), clipping based on this is done.
// - As clipping occurs, if in line or triangle mode, lines and triangles are written to an index buffer
// - Extra points generated from clipping are inserted into an extra buffer.

enum class RenderMode : uint8_t
{
	Point,
	Line,
	Triangle
};

enum class RenderObject : uint8_t
{
	Mesh,
	Material,
	Buffer,
	RenderMode,
	AlphaBlending,
	ColourInterpolation,
	PositionInterpolation,
	NormalInterpolation,
	TangentInterpolation,
	BitangentInterpolation,
	FrontFaceCulling,
	BackFaceCulling,
	DepthTest,
	Clipping,
};

struct DrawCall
{
	Mesh* Mesh;
	Material* Material;
	RenderMode Mode;
	Matrix4* ModelTransform;
};

typedef uint32_t RenderBufferHandle;

class RenderBuffer
{
public:

	class CIterator;

	class Iterator
	{
	private:

		Iterator( void* a_Data, size_t a_Stride )
			: m_Data( a_Data )
			, m_Stride( a_Stride )
		{ }

	public:

		using iterator_category = std::forward_iterator_tag;
		using value_type = void*;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		inline void* operator++()
		{
			return ( m_Data = reinterpret_cast< uint8_t* >( m_Data ) + m_Stride );
		}

		inline void* operator++( int )
		{
			void* Temp = m_Data;
			m_Data = reinterpret_cast< uint8_t* >( m_Data ) + m_Stride;
			return Temp;
		}

		void* operator*()
		{
			return m_Data;
		}

		void* operator->()
		{
			return m_Data;
		}

		inline bool operator==( const Iterator& a_Iterator ) const
		{
			return m_Data == a_Iterator.m_Data;
		}

		inline bool operator==( const CIterator& a_Iterator ) const
		{
			return m_Data == a_Iterator.m_Data;
		}

		inline bool operator!=( const Iterator& a_Iterator ) const
		{
			return !( *this == a_Iterator );
		}

		inline bool operator!=( const CIterator& a_Iterator ) const
		{
			return !( *this == a_Iterator );
		}

		template < typename T >
		inline T* Read( size_t a_Offset )
		{
			return reinterpret_cast< T* >( reinterpret_cast< uint8_t* >( m_Data ) + a_Offset );
		}

		template < typename T >
		inline const T* Read( size_t a_Offset ) const
		{
			return reinterpret_cast< const T* >( reinterpret_cast< const uint8_t* >( m_Data ) + a_Offset );
		}

		template < typename T >
		void Write( const T& a_Value, size_t a_Offset )
		{
			memcpy( reinterpret_cast< uint8_t* >( m_Data ) + a_Offset, &a_Value, sizeof( T ) );
		}

	private:

		friend class CIterator;
		friend class RenderBuffer;

		void*  m_Data;
		size_t m_Stride;
	};

	class CIterator
	{
	private:

		CIterator( const void* a_Data, size_t a_Stride )
			: m_Data( a_Data )
			, m_Stride( a_Stride )
		{}

	public:

		using iterator_category = std::forward_iterator_tag;
		using value_type = const void*;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		inline const void* operator++()
		{
			return ( m_Data = reinterpret_cast< const uint8_t* >( m_Data ) + m_Stride );
		}

		inline const void* operator++( int )
		{
			const void* Temp = m_Data;
			m_Data = reinterpret_cast< const uint8_t* >( m_Data ) + m_Stride;
			return Temp;
		}

		const void* operator*()
		{
			return m_Data;
		}

		const void* operator->()
		{
			return m_Data;
		}

		inline bool operator==( const Iterator& a_Iterator ) const
		{
			return m_Data == a_Iterator.m_Data;
		}

		inline bool operator==( const CIterator& a_Iterator ) const
		{
			return m_Data == a_Iterator.m_Data;
		}

		inline bool operator!=( const Iterator& a_Iterator ) const
		{
			return !( *this == a_Iterator );
		}

		inline bool operator!=( const CIterator& a_Iterator ) const
		{
			return !( *this == a_Iterator );
		}

		template < typename T >
		inline const T* Read( size_t a_Offset ) const
		{
			return reinterpret_cast< const T* >( reinterpret_cast< const uint8_t* >( m_Data ) + a_Offset );
		}

	private:

		friend class Iterator;
		friend class RenderBuffer;

		const void* m_Data;
		size_t m_Stride;
	};

	inline Iterator Begin()
	{
		return Iterator( m_Buffer.data(), m_Stride );
	}

	inline CIterator Begin() const
	{
		return CIterator( m_Buffer.data(), m_Stride );
	}

	inline CIterator CBegin() const
	{
		return CIterator( m_Buffer.data(), m_Stride );
	}

	inline Iterator End()
	{
		return Iterator( m_Buffer.data() + m_Buffer.size(), m_Stride );
	}

	inline CIterator End() const
	{
		return CIterator( m_Buffer.data() + m_Buffer.size(), m_Stride );
	}

	inline CIterator CEnd() const
	{
		return CIterator( m_Buffer.data() + m_Buffer.size(), m_Stride );
	}

	inline void Reset( size_t a_Size, size_t a_Stride )
	{
		m_Buffer.reserve( a_Size );
		m_Buffer.clear();
	}

private:

	size_t m_Stride;
	std::vector< uint8_t > m_Buffer;
};

struct RenderState
{
	uint32_t   ActiveMesh;
	uint32_t   ActiveMaterial;
	uint8_t    ActiveRenderTarget;
	uint8_t    ActiveInputBuffer            : 3;
	uint8_t    ActiveOutputBuffer           : 3;
	RenderMode ActiveRenderMode             : 2;
	bool       AlphaBlendingActive          : 1;
	bool       ColourInterpolationActive    : 1;
	bool       PositionInterpolationActive  : 1;
	bool       NormalInterpolationActive    : 1;
	bool       TangentInterpolationActive   : 1;
	bool       BitangentInterpolationActive : 1;
	bool       FrontFaceCullingActive       : 1;
	bool       BackFaceCullingActive        : 1;
	bool       DepthTestingActive           : 1;
	bool       ClippingActive               : 1;
};

enum class RenderCommand
{

};

struct RenderInstruction
{

};

class Rendering
{
private:

	static RenderBufferHandle CreateRenderBuffer()
	{

	}

	static bool DestroyRenderBuffer( RenderBufferHandle a_Handle )
	{

	}

	static RenderBuffer* GetRenderBuffer( RenderBufferHandle a_Handle )
	{

	}

	static bool SwitchRenderBuffers( RenderBufferHandle a_HandleA, RenderBufferHandle a_HandleB )
	{
		return false;
	}

	static std::vector< RenderBuffer > s_RenderBuffers;
};