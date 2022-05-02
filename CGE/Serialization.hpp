#pragma once
#include <type_traits>
#include <vector>

class StreamSizer;
class SerializationStream;
class DeserializationStream;

class IStreamSizeable
{
protected:

	virtual void SizeOf( StreamSizer& a_StreamSizer ) const = 0;
};

class ISerializable
{
protected:

	virtual void Serialize( SerializationStream& a_Stream ) const = 0;
};

class IDeserializable
{
protected:

	virtual void Deserialize( const DeserializationStream& a_Stream ) = 0;
};

class StreamSizer
{
public:

	StreamSizer()
		: m_Size( 0 )
	{ }

	template < typename T >
	std::enable_if_t< !std::is_base_of_v< IStreamSizeable, T >, StreamSizer& > operator+=( const T& a_Sizeable )
	{
		m_Size += sizeof( T );
		return *this;
	}

	template < typename T >
	std::enable_if_t< std::is_base_of_v< IStreamSizeable, T >, StreamSizer& > operator+=( const T& a_Sizeable )
	{
		a_Sizeable.SizeOf( *this );
		return *this;
	}

	inline void ExplicitAdd( size_t a_Size )
	{
		m_Size += a_Size;
	}

private:

	size_t Size() const
	{
		return m_Size;
	}

	friend class FileSerializer;
	friend class FileDeserializer;

	size_t m_Size;
};

class SerializationStream
{
private:

	void Reset( void* a_Stream )
	{
		m_Head = reinterpret_cast< uint8_t* >( a_Stream );
	}

	template < typename T >
	friend std::enable_if_t< !std::is_base_of_v< ISerializable, T >, SerializationStream& > operator <<( SerializationStream& a_Stream, const T& a_In )
	{
		memcpy_s( a_Stream.m_Head, sizeof( T ), &a_In, sizeof( T ) );
		a_Stream.m_Head += sizeof( T );
		return a_Stream;
	}

	template < typename T >
	friend std::enable_if_t< std::is_base_of_v< ISerializable, T >, SerializationStream& > operator <<( SerializationStream& a_Stream, const T& a_In )
	{
		a_In.Serialize( a_Stream );
		return a_Stream;
	}

	inline const uint8_t* Data() const
	{
		return m_Head;
	}

	friend class FileSerializer;

	uint8_t* m_Head;
};

class DeserializationStream
{
public:

	void Reset( void* a_Stream )
	{
		m_Head = reinterpret_cast< uint8_t* >( a_Stream );
	}

	template < typename T >
	friend std::enable_if_t< !std::is_base_of_v< IDeserializable, T >, const DeserializationStream& > operator >>( const DeserializationStream& a_Stream, T& a_Out )
	{
		memcpy_s( &a_Out, sizeof( T ), a_Stream.m_Head, sizeof( T ) );
		const_cast< DeserializationStream* >( &a_Stream )->m_Head += sizeof( a_Out );
		return a_Stream;
	}

	template < typename T >
	friend std::enable_if_t< std::is_base_of_v< IDeserializable, T >, const DeserializationStream& > operator >>( const DeserializationStream& a_Stream, T& a_Out )
	{
		a_Out.Deserialize( a_Stream );
		return a_Stream;
	}

	inline const uint8_t* Data() const
	{
		return m_Head;
	}

private:

	uint8_t* m_Head;
};