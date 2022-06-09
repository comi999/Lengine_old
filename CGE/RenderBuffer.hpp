#pragma once
#include <stdint.h>
#include <iterator>
#include <vector>

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
		{}

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

		void* m_Data;
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

	inline void Rewind( size_t a_Size, size_t a_Stride )
	{
		m_Buffer.resize( a_Size * a_Stride );
		m_Stride = a_Stride;
	}

	inline size_t Size() const
	{
		return m_Buffer.size() / m_Stride;
	}

	inline size_t RawSize() const
	{
		return m_Buffer.size();
	}

	inline uint8_t* operator[]( size_t a_Index )
	{
		return &m_Buffer[ a_Index * m_Stride ];
	}

private:

	void Swap( RenderBuffer& a_RenderBuffer )
	{
		m_Buffer.swap( a_RenderBuffer.m_Buffer );
		std::swap( a_RenderBuffer.m_Stride, m_Stride );
	}

	friend class RenderPipeline;

	size_t                 m_Stride;
	std::vector< uint8_t > m_Buffer;
};