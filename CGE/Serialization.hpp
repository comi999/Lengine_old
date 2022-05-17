#pragma once
#include <stdio.h>
#include <type_traits>
#include <string>
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <stack>
#include <queue>

template < typename >
class Serializer;

template < typename >
class Deserializer;

class StreamSizer;

class Serialization
{
	#pragma region HasOnBeforeSerialize

	template < typename T >
	static constexpr auto _HasOnBeforeSerializeImpl( T* ) ->
		typename std::is_same< decltype( std::declval< T& >().OnBeforeSerialize() ), void >::type;

	template < typename >
	static constexpr std::false_type _HasOnBeforeSerializeImpl( ... );

	template < typename T >
	using _HasOnBeforeSerialize = decltype( _HasOnBeforeSerializeImpl< T >( 0 ) );

	#pragma endregion

	#pragma region HasSerialize

	template < typename T >
	static constexpr auto _HasSerializeImpl( T* ) ->
		typename std::is_same< decltype( std::declval< T& >().Serialize( std::declval< void*& >() ) ), void >::type;

	template < typename >
	static constexpr std::false_type _HasSerializeImpl( ... );

	template < typename T >
	using _HasSerialize = decltype( _HasSerializeImpl< T >( 0 ) );

	#pragma endregion

	#pragma region HasOnAfterSerialize

	template < typename T >
	static constexpr auto _HasOnAfterSerializeImpl( T* ) ->
		typename std::is_same< decltype( std::declval< T& >().OnAfterSerialize() ), void >::type;

	template < typename >
	static constexpr std::false_type _HasOnAfterSerializeImpl( ... );

	template < typename T >
	using _HasOnAfterSerialize = decltype( _HasOnAfterSerializeImpl< T >( 0 ) );

	#pragma endregion

	#pragma region HasOnBeforeDeserialize

	template < typename T >
	static constexpr auto _HasOnBeforeDeserializeImpl( T* ) ->
		typename std::is_same< decltype( std::declval< T& >().OnBeforeDeserialize() ), void >::type;

	template < typename >
	static constexpr std::false_type _HasOnBeforeDeserializeImpl( ... );

	template < typename T >
	using _HasOnBeforeDeserialize = decltype( _HasOnBeforeDeserializeImpl< T >( 0 ) );

	#pragma endregion

	#pragma region HasDeserialize

	template < typename T >
	static constexpr auto _HasDeserializeImpl( T* ) ->
		typename std::is_same< decltype( std::declval< T& >().Deserialize( std::declval< void*& >() ) ), void >::type;

	template < typename >
	static constexpr std::false_type _HasDeserializeImpl( ... );

	template < typename T >
	using _HasDeserialize = decltype( _HasDeserializeImpl< T >( 0 ) );

	#pragma endregion

	#pragma region HasOnAfterDeserialize

	template < typename T >
	static constexpr auto _HasOnAfterDeserializeImpl( T* ) ->
		typename std::is_same< decltype( std::declval< T& >().OnAfterDeserialize() ), void >::type;

	template < typename >
	static constexpr std::false_type _HasOnAfterDeserializeImpl( ... );

	template < typename T >
	using _HasOnAfterDeserialize = decltype( _HasOnAfterDeserializeImpl< T >( 0 ) );

	#pragma endregion

	#pragma region HasSizeOf

	template < typename T >
	static constexpr auto _HasSizeOfImpl( T* ) ->
		typename std::is_same< decltype( std::declval< T& >().SizeOf( std::declval< void*& >() ) ), void >::type;

	template < typename >
	static constexpr std::false_type _HasSizeOfImpl( ... );

	template < typename T >
	using _HasSizeOf = decltype( _HasSizeOfImpl< T >( 0 ) );

	#pragma endregion

public:

	template < typename T >
	static constexpr bool HasOnBeforeSerialize = _HasOnBeforeSerialize< T >::value;

	template < typename T >
	static constexpr bool HasSerialize = _HasSerialize< T >::value;

	template < typename T >
	static constexpr bool HasOnAfterSerialize = _HasOnAfterSerialize< T >::value;

	template < typename T >
	static constexpr bool HasOnBeforeDeserialize = _HasOnBeforeDeserialize< T >::value;

	template < typename T >
	static constexpr bool HasDeserialize = _HasDeserialize< T >::value;

	template < typename T >
	static constexpr bool HasOnAfterDeserialize = _HasOnAfterDeserialize< T >::value;

	template < typename T >
	static constexpr bool HasSizeOf = _HasSizeOf< T >::value;

	template < typename _Serializable, typename _Serializer >
	inline static void Serialize( const _Serializable& a_Serializable, _Serializer& a_Serializer )
	{
		if constexpr ( HasOnBeforeSerialize< _Serializable > )
		{
			const_cast< _Serializable& >( a_Serializable ).OnBeforeSerialize();
		}

		if constexpr ( HasSerialize< _Serializable > )
		{
			a_Serializable.Serialize( a_Serializer );
		}
		else
		{
			Serializer< _Serializable >( a_Serializable ).Serialize( a_Serializer );
		}

		if constexpr ( HasOnAfterSerialize< _Serializable > )
		{
			const_cast< _Serializable& >( a_Serializable ).OnAfterSerialize();
		}
	}

	template < typename _Deserializable, typename _Deserializer >
	inline static void Deserialize( _Deserializable& a_Deserializable, _Deserializer& a_Deserializer )
	{
		if constexpr ( HasOnBeforeDeserialize< _Deserializable > )
		{
			const_cast< _Deserializable& >( a_Deserializable ).OnBeforeDeserialize();
		}

		if constexpr ( HasDeserialize< _Deserializable > )
		{
			a_Deserializable.Deserialize( a_Deserializer );
		}
		else
		{
			Deserializer< _Deserializable >( a_Deserializable ).Deserialize( a_Deserializer );
		}

		if constexpr ( HasOnAfterDeserialize< _Deserializable > )
		{
			const_cast< _Deserializable& >( a_Deserializable ).OnAfterDeserialize();
		}
	}

	template < typename _Sizeable, typename _Sizer >
	inline static void SizeOf( const _Sizeable& a_Sizeable, _Sizer& a_Sizer )
	{
		if constexpr ( HasSizeOf< _Sizeable > )
		{
			a_Sizeable.SizeOf( a_Sizer );
		}
		else
		{
			Serializer< _Sizeable >( a_Sizeable ).SizeOf( a_Sizer );
		}
	}

	template < typename _Sizeable >
	inline static size_t GetSizeOf( const _Sizeable& a_Sizeable )
	{
		StreamSizer Sizer;
		SizeOf( a_Sizeable, Sizer );
		return Sizer;
	}

private:

	Serialization( Serialization&& ) = delete;
};

template < typename _Stream >
class StreamSerializer
{
	typedef _Stream                     _Base;
	typedef StreamSerializer< _Stream > _This;

public:

	StreamSerializer( _Stream& a_Stream )
		: m_Stream( a_Stream )
	{ }

	template < typename T >
	_This& operator << ( const T& a_Serializable )
	{
		Serialization::Serialize( a_Serializable, *this );
		return *this;
	}

	_Stream& Stream()
	{
		return m_Stream;
	}

	const _Stream& Stream() const
	{
		return m_Stream;
	}

private:

	template < typename > friend class Serializer;

	StreamSerializer( StreamSerializer&& ) = delete;

	_Stream& m_Stream;
};

template < typename _Stream >
class StreamDeserializer
{
	typedef _Stream                       _Base;
	typedef StreamDeserializer< _Stream > _This;

public:

	StreamDeserializer( _Stream& a_Stream )
		: m_Stream( a_Stream )
	{ }

	template < typename T >
	_This& operator >> ( T& a_Deserializable )
	{
		Serialization::Deserialize( a_Deserializable, *this );
		return *this;
	}

	_Stream& Stream()
	{
		return m_Stream;
	}

	const _Stream& Stream() const
	{
		return m_Stream;
	}

private:

	template < typename > friend class Deserializer;

	StreamDeserializer( StreamDeserializer&& ) = delete;

	_Stream& m_Stream;
};

class StreamSizer
{
public:

	StreamSizer()
		: m_Size( 0 )
	{ }

	template < typename T >
	StreamSizer& operator &( const T& a_Object )
	{
		Serialization::SizeOf( a_Object, *this );
		return *this;
	}

	StreamSizer& operator +=( size_t a_Size )
	{
		m_Size += a_Size;
		return *this;
	}

	operator size_t () const
	{
		return m_Size;
	}

private:

	size_t m_Size;
};

template < typename T >
class Serializer
{
	using Type = T;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{ }

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{ }

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer.m_Stream.Write( m_Serializable, sizeof( Type ) );
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( T );
	}

	const Type* m_Serializable;
};

template < typename T >
class Deserializer
{
	using Type = T;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{ }

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{ }

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		a_Deserializer.m_Stream.Read( m_Deserializable, sizeof( Type ) );
	}

	Type* m_Deserializable;
};

class BufferStream
{
public:

	BufferStream()
		: m_Data( nullptr )
		, m_Head( nullptr )
		, m_Size( 0 )
	{ }

	BufferStream( size_t a_Size )
		: m_Data( ( uint8_t* )malloc( a_Size ) )
		, m_Head( m_Data )
		, m_Size( a_Size )
	{ }

	~BufferStream()
	{
		Close();
	}

	void Open( size_t a_Size )
	{
		if ( m_Data )
		{
			Close();
		}

		m_Data = ( uint8_t* )malloc( a_Size );
		m_Head = m_Data;
		m_Size = a_Size;
	}

	void Close()
	{
		delete[] m_Data;
		m_Data = nullptr;
		m_Head = nullptr;
		m_Size = 0;
	}

	inline void Write( const void* a_From, size_t a_Size )
	{
		memcpy( m_Head, a_From, a_Size );
		m_Head += a_Size;
	}

	inline void Read( void* a_To, size_t a_Size )
	{
		memcpy( a_To, m_Head, a_Size );
		m_Head += a_Size;
	}

	inline void Seek( size_t a_Position )
	{
		m_Head = m_Data + a_Position;
	}

	inline bool End() const
	{
		return m_Head - m_Data == m_Size;
	}

	inline size_t Size() const
	{
		return m_Size;
	}

	inline void* Data()
	{
		return m_Data;
	}

	inline const void* Data() const
	{
		return m_Data;
	}

private:

	uint8_t* m_Data;
	uint8_t* m_Head;
	size_t   m_Size;
};

typedef StreamSerializer  < BufferStream > BufferSerializer;
typedef StreamDeserializer< BufferStream > BufferDeserializer;

template < typename >
class Serializer;

template < typename >
class Deserializer;

template < typename... Args >
class Serializer< std::pair< Args... > >
{
	using Type = std::pair< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->first << m_Serializable->second;
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer& m_Serializable->first& m_Serializable->second;
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::pair< Args... > >
{
	using Type = std::pair< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		a_Deserializer >> m_Deserializable->first >> m_Deserializable->second;
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::tuple< Args... > >
{
	using Type = std::tuple< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		if constexpr ( sizeof...( Args ) > 0 )
		{
			a_Serializer << m_Serializable->_Myfirst._Val;
		}

		if constexpr ( sizeof...( Args ) > 1 )
		{
			a_Serializer << m_Serializable->_Get_rest();
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		if constexpr ( sizeof...( Args ) > 0 )
		{
			a_Sizer& m_Serializable->_Myfirst._Val;
		}

		if constexpr ( sizeof...( Args ) > 1 )
		{
			a_Sizer& m_Serializable->_Get_rest();
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::tuple< Args... > >
{
	using Type = std::tuple< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		if constexpr ( sizeof...( Args ) > 0 )
		{
			a_Deserializer >> m_Deserializable->_Myfirst._Val;
		}

		if constexpr ( sizeof...( Args ) > 1 )
		{
			a_Deserializer >> m_Deserializable->_Get_rest();
		}
	}

	Type* m_Deserializable;
};

template < typename T, size_t _Size >
class Serializer< T( * )[ _Size ] >
{
	using Type = T( * )[ _Size ];

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << _Size;

		for ( size_t i = 0; i < _Size; ++i )
		{
			a_Serializer << ( *m_Serializable )[ i ];
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( size_t i = 0; i < _Size; ++i )
		{
			a_Sizer& ( *m_Serializable )[ i ];
		}
	}

	const Type* m_Serializable;
};

template < typename T, size_t _Size >
class Deserializer< T( * )[ _Size ] >
{
	using Type = T( * )[ _Size ];

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;
		Size = Size > _Size ? _Size : Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			a_Deserializer >> ( *m_Deserializable )[ i ];
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::basic_string< Args... > >
{
	using Type = std::basic_string< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();
		a_Serializer.m_Stream.Write( m_Serializable->data(), sizeof( typename Type::value_type ) * m_Serializable->size() );
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );
		a_Sizer += sizeof( typename Type::value_type ) * m_Serializable->size();
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::basic_string< Args... > >
{
	using Type = std::basic_string< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;
		m_Deserializable->resize( Size );
		a_Deserializer.m_Stream.Read( m_Deserializable->data(), sizeof( typename Type::value_type ) * Size );
	}

	Type* m_Deserializable;
};

template < typename T, size_t _Size >
class Serializer< std::array< T, _Size > >
{
	using Type = std::array< T, _Size >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << _Size;

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename T, size_t _Size >
class Deserializer< std::array< T, _Size > >
{
	using Type = std::array< T, _Size >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;
		Size = Size > _Size ? _Size : Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			a_Deserializer >> m_Deserializable->at( i );
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::vector< Args... > >
{
	using Type = std::vector< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::vector< Args... > >
{
	using Type = std::vector< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;
		m_Deserializable->resize( Size );

		for ( auto& Element : *m_Deserializable )
		{
			a_Deserializer >> Element;
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::deque< Args... > >
{
	using Type = std::deque< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );
		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::deque< Args... > >
{
	using Type = std::deque< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			a_Deserializer >> m_Deserializable->emplace_back();
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::forward_list< Args... > >
{
	using Type = std::forward_list< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::forward_list< Args... > >
{
	using Type = std::forward_list< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			a_Deserializer >> m_Deserializable->emplace_back();
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::list< Args... > >
{
	using Type = std::list< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::list< Args... > >
{
	using Type = std::list< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			a_Deserializer >> m_Deserializable->emplace_back();
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::map< Args... > >
{
	using Type = std::map< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Pair : *m_Serializable )
		{
			a_Serializer << Pair;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::map< Args... > >
{
	using Type = std::map< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			typename Type::key_type Key;
			a_Deserializer >> Key;
			a_Deserializer >> ( *m_Deserializable )[ Key ];
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::multimap< Args... > >
{
	using Type = std::multimap< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Pair : *m_Serializable )
		{
			a_Serializer << Pair;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::multimap< Args... > >
{
	using Type = std::multimap< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			typename Type::key_type Key;
			a_Deserializer >> Key;
			a_Deserializer >> ( *m_Deserializable )[ Key ];
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::unordered_map< Args... > >
{
	using Type = std::unordered_map< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Pair : *m_Serializable )
		{
			a_Serializer << Pair;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::unordered_map< Args... > >
{
	using Type = std::unordered_map< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			typename Type::key_type Key;
			a_Deserializer >> Key;
			a_Deserializer >> ( *m_Deserializable )[ Key ];
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::unordered_multimap< Args... > >
{
	using Type = std::unordered_multimap< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Pair : *m_Serializable )
		{
			a_Serializer << Pair;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::unordered_multimap< Args... > >
{
	using Type = std::unordered_multimap< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			typename Type::key_type Key;
			a_Deserializer >> Key;
			a_Deserializer >> ( *m_Deserializable )[ Key ];
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::set< Args... > >
{
	using Type = std::set< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::set< Args... > >
{
	using Type = std::set< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			typename Type::value_type Value;
			a_Deserializer >> Value;
			m_Deserializable->emplace( Value );
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::multiset< Args... > >
{
	using Type = std::multiset< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::multiset< Args... > >
{
	using Type = std::multiset< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			typename Type::value_type Value;
			a_Deserializer >> Value;
			m_Deserializable->emplace( Value );
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::unordered_set< Args... > >
{
	using Type = std::unordered_set< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::unordered_set< Args... > >
{
	using Type = std::unordered_set< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			typename Type::value_type Value;
			a_Deserializer >> Value;
			m_Deserializable->emplace( Value );
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::unordered_multiset< Args... > >
{
	using Type = std::unordered_multiset< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->size();

		for ( auto& Element : *m_Serializable )
		{
			a_Serializer << Element;
		}
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer += sizeof( size_t );

		for ( auto& Element : *m_Serializable )
		{
			a_Sizer& Element;
		}
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::unordered_multiset< Args... > >
{
	using Type = std::unordered_multiset< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		size_t Size;
		a_Deserializer >> Size;

		for ( size_t i = 0; i < Size; ++i )
		{
			typename Type::value_type Value;
			a_Deserializer >> Value;
			m_Deserializable->emplace( Value );
		}
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::stack< Args... > >
{
	using Type = std::stack< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->_Get_container();
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer& m_Serializable->_Get_container();
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::stack< Args... > >
{
	using Type = std::stack< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		a_Deserializer >> const_cast< typename Type::container_type& >( m_Deserializable->_Get_container() );
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::queue< Args... > >
{
	using Type = std::queue< Args... >;

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << m_Serializable->_Get_container();
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer& m_Serializable->_Get_container();
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::queue< Args... > >
{
	using Type = std::queue< Args... >;

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		a_Deserializer >> const_cast< typename Type::container_type& >( m_Deserializable->_Get_container() );
	}

	Type* m_Deserializable;
};

template < typename... Args >
class Serializer< std::priority_queue< Args... > >
{
	using Type = std::priority_queue< Args... >;

	struct Access : Type
	{
		auto& _Get_Container() const
		{
			return Type::c;
		}
	};

public:

	Serializer( const Type& a_Serializable )
		: m_Serializable( &a_Serializable )
	{}

	Serializer( const Type* a_Serializable )
		: m_Serializable( a_Serializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamSerializer >
	void Serialize( _StreamSerializer& a_Serializer ) const
	{
		a_Serializer << reinterpret_cast< const Access* >( m_Serializable )->_Get_Container();
	}

	template < typename _Sizer >
	void SizeOf( _Sizer& a_Sizer ) const
	{
		a_Sizer& reinterpret_cast< const Access* >( m_Serializable )->_Get_Container();
	}

	const Type* m_Serializable;
};

template < typename... Args >
class Deserializer< std::priority_queue< Args... > >
{
	using Type = std::priority_queue< Args... >;

	struct Access : Type
	{
		auto& _Get_Container()
		{
			return Type::c;
		}
	};

public:

	Deserializer( Type& a_Deserializable )
		: m_Deserializable( &a_Deserializable )
	{}

	Deserializer( Type* a_Deserializable )
		: m_Deserializable( a_Deserializable )
	{}

private:

	friend class Serialization;

	template < typename _StreamDeserializer >
	void Deserialize( _StreamDeserializer& a_Deserializer ) const
	{
		a_Deserializer >> reinterpret_cast< Access* >( m_Deserializable )->_Get_Container();
	}

	Type* m_Deserializable;
};