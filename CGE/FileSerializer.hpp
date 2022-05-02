#include <stdio.h>
#include "Serialization.hpp"

class FileSerializer
{
public:

	FileSerializer()
		: m_File( nullptr )
	{ }

	bool Open( const char* a_Path )
	{
		if ( m_File || !a_Path )
		{
			return false;
		}

		return fopen_s( &m_File, a_Path, "w+" );
	}

	bool Close()
	{
		if ( !m_File )
		{
			return false;
		}

		bool Success = !fclose( m_File );

		if ( Success )
		{
			m_File = nullptr;
		}

		return Success;
	}

	template < typename T >
	bool Write( const T& a_Data )
	{
		if ( !m_File )
		{
			return false;
		}

		StreamSizer Sizer;
		Sizer += a_Data;
		m_Buffer.resize( Sizer.Size() );
		SerializationStream Stream;
		Stream.Reset( m_Buffer.data() );
		Stream << a_Data;
		return fwrite( m_Buffer.data(), sizeof( uint8_t ), m_Buffer.size(), m_File );
	}

	inline bool Seek( size_t a_Offset )
	{
		return !fseek( m_File, a_Offset, 0 );
	}

private:

	FILE*                  m_File;
	SerializationStream    m_Stream;
	std::vector< uint8_t > m_Buffer;
};

class FileDeserializer
{
public:

	FileDeserializer()
		: m_File( nullptr )
	{ }

	bool Open( const char* a_Path )
	{
		if ( m_File || !a_Path )
		{
			return false;
		}
		
		bool Success = !fopen_s( &m_File, a_Path, "r" );

		if ( !Success || !m_File )
		{
			return false;
		}

		fseek( m_File, 0, SEEK_END );
		size_t Size = ftell( m_File );
		m_Buffer.resize( Size );
		return true;
	}

	bool Close()
	{
		if ( !m_File )
		{
			return false;
		}

		bool Success = !fclose( m_File );

		if ( Success )
		{
			m_File = nullptr;
		}

		return Success;
	}

	template < typename T >
	bool Read( T& a_Data )
	{
		if ( !m_File )
		{
			return false;
		}

		fread( m_Buffer.data(), sizeof( uint8_t ), m_Buffer.size(), m_File );
		DeserializationStream Stream;
		Stream.Reset( m_Buffer.data() );
		Stream >> a_Data;
	}

	inline bool Seek( size_t a_Offset )
	{
		return !fseek( m_File, a_Offset, 0 );
	}

private:

	FILE*				   m_File;
	DeserializationStream  m_Stream;
	std::vector< uint8_t > m_Buffer;
};