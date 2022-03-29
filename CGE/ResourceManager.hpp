#pragma once
#include <cstdint>

class ResourceManager
{
private:

	class TypeIndexer
	{
	public:

		static size_t Next()
		{
			static size_t Index = 0;
			return Index++;
		}

		template < typename T >
		static size_t GetTypeIndex()
		{
			static size_t Index = Next();
			return Index;
		}
	};

	struct ResourcePool
	{
	public:

		ResourcePool()
			: m_Destructor( nullptr )
			, m_Data( nullptr )
			, m_Size( 0 )
		{ }

		template < typename T >
		void Initialize( size_t a_Size )
		{
			m_Destructor = Destructor< T >;
			m_Data = new T[ a_Size ];
			m_Size = a_Size;
		}

		void Reset()
		{
			
		}

	private:

		template < typename T >
		static void Destructor( void* a_Instance )
		{
			delete reinterpret_cast< T* >( a_Instance );
		}

		void( *m_Destructor )( void* );
		uint8_t* m_Data;
		size_t   m_Size;
	};

public:


};