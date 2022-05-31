#pragma once
#include "Math.hpp"
#include "File.hpp"
#include "Colour.hpp"

class Texture
{
public:

	Texture()
		: m_Data( nullptr )
	{ }

	Texture( Vector2Int a_Size )
		: m_Size( a_Size )
		, m_Data( new Colour[ a_Size.x * a_Size.y ] )
	{ }

	Texture( Vector2Int a_Size, Colour a_Colour )
		: m_Size( a_Size )
		, m_Data( new Colour[ a_Size.x * a_Size.y ] { a_Colour } )
	{ }

	Texture( Colour a_Colour )
		: m_Size( Vector2Int::One )
		, m_Data( new Colour( a_Colour ) )
	{ }

	Texture( const File& a_Path )
	{
		// Need to load a .texture file
	}

	~Texture()
	{
		
	}

	inline Vector2Int GetDimensions() const
	{
		return m_Size;
	}

	inline int GetWidth() const
	{
		return m_Size.x;
	}

	inline int GetHeight() const
	{
		return m_Size.y;
	}

	inline Colour* GetData()
	{
		return m_Data;
	}

	const Colour* GetData() const
	{

	}

	inline Colour operator[]( size_t a_Index ) const
	{
		return m_Data[ a_Index ];
	}

	inline Colour Sample( Vector2 a_UV ) const
	{
		int Index = 
			static_cast< int >( ( m_Size.y - 1 ) * a_UV.y ) * m_Size.x + 
			static_cast< int >( ( m_Size.x - 1 ) * a_UV.x );
		return m_Data[ Index ];
	}

private:

	friend class ResourcePackager;
	friend class Serialization;

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << m_Size;
		a_Serializer.Stream().Write( m_Data, sizeof( Colour ) * m_Size.x * m_Size.y );
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		if ( m_Data )
		{
			delete[] m_Data;
		}

		a_Deserializer >> m_Size;
		m_Data = new Colour[ m_Size.x * m_Size.y ];
		a_Deserializer.Stream().Read( m_Data, sizeof( Colour ) * m_Size.x * m_Size.y );
	}

	template < typename T >
	void SizeOf( T& a_Sizer ) const
	{
		a_Sizer & m_Size;
		a_Sizer += m_Size.x * m_Size.y * sizeof( Colour );
	}

	Vector2Int   m_Size;
	Colour*      m_Data;
};