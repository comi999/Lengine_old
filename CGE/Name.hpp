#pragma once
#include <string>
#include <list>
#include "Hash.hpp"


struct Name
{
public:

    Name() 
        : m_String( nullptr )
        , m_Length( 0 )
        , m_Hash( 0 ) 
    {}

    template < size_t N >
    constexpr Name( const char( &C )[ N ] )
        : m_String( C )
        , m_Length( N - 1 )
        , m_Hash( crc32_cpt< N >( C ) )
    {}

    constexpr Name( const char* C, size_t N, Hash H )
        : m_String( C )
        , m_Length( int32_t( N ) )
        , m_Hash( H )
    {}

    Name( char* a_String )
        : m_String( a_String )
        , m_Length( a_String ? ( int32_t( strlen( a_String ) ) ) : 0 )
        , m_Hash( a_String ? crc32_impl< sizeof( const char* ) >( a_String ) : 0 )
    {
        if ( a_String )
        {
            s_NameStorage.push_back( a_String );
            m_String = s_NameStorage.back().c_str();
        }
    }

    Name( char* a_String, size_t a_Length )
        : m_String( a_String )
        , m_Length( int32_t( a_Length ) )
        , m_Hash( crc32_impl< sizeof( const char* ) >( a_String ) )
    {
        s_NameStorage.push_back( a_String );
        m_String = s_NameStorage.back().c_str();
    }

    Name( const std::string& a_String )
        : m_String( a_String.c_str() ), m_Length( int32_t( a_String.length() ) ),
        m_Hash( crc32_impl< sizeof( const char* ) >( m_String ) )
    {
        s_NameStorage.push_back( a_String );
        m_String = s_NameStorage.back().c_str();
    }

    operator std::string() const { return std::string( m_String, m_Length ); }
    operator Hash() const { return m_Hash; }
    operator const char*() const { return m_String; }
    constexpr char operator []( size_t a_Pos ) { return m_String[ a_Pos ]; }

    constexpr const char* Data() const { return m_String; }
    constexpr int32_t Length() const { return m_Length; }
    constexpr Hash HashCode() const { return m_Hash; }

    constexpr bool operator==( Hash a_CRC32 ) const
    {
        return m_Hash == a_CRC32;
    }

    constexpr bool operator==( const Name& a_Name ) const
    {
        return m_Hash == a_Name.m_Hash;
    }

    constexpr bool operator!=( Hash a_CRC32 ) const
    {
        return m_Hash != a_CRC32;
    }

    constexpr bool operator!=( const Name& a_Name ) const
    {
        return m_Hash != a_Name.m_Hash;
    }

private:

    friend class Serialization;

    template < typename _Serializer >
    void Serialize( _Serializer& a_Serializer ) const
    {
        a_Serializer << m_Hash << m_Length;
        a_Serializer.Stream().Write( m_String, sizeof( char ) * m_Length );
    }

    template < typename _Deserializer >
    void Deserialize( _Deserializer& a_Deserializer )
    {
        a_Deserializer >> m_Hash >> m_Length;
        s_NameStorage.emplace_back();
        auto& String = s_NameStorage.back();
        String.resize( m_Length );
        a_Deserializer.Stream().Read( String.data(), sizeof( char ) * m_Length );
        m_String = String.data();
    }

    template < typename _Sizer >
    void SizeOf( _Sizer& a_Sizer ) const
    {
        a_Sizer & m_Hash & m_Length += sizeof( const char ) * m_Length;
    }

    inline static std::list< std::string > s_NameStorage;
    const char* m_String;
    int32_t m_Length;
    Hash m_Hash;
};

constexpr Name
operator "" _N( const char* C, size_t N )
{
    return Name( C, N, crc32_impl2( C, N - 1 ) ^ 0xFFFFFFFF );
}

namespace std
{
    template<>
	struct hash< Name >
	{
		inline size_t operator()( const Name& a_Value ) const
		{
            return a_Value.HashCode();
		}
	};
}