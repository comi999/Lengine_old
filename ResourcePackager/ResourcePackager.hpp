#pragma once
#include "File.hpp"
#include "Prefab.hpp"

class ResourceEntry;

class ResourcePackager
{
public:

	ResourcePackager( 
		Directory a_Source = Directory(), 
		Directory a_Output = Directory() )
		: m_Source( a_Source )
		, m_Output( a_Output )
	{ }

	bool Build() const;

private:

	friend File ProcessResourceEntry( ResourceEntry&, Directory& );

	// Helper functions to access resource private members:
	inline static Vector2Int& GetTextureSize( Texture2D& a_Texture )
	{
		return a_Texture.m_Size;
	}
	inline static uint8_t*& GetTextureData( Texture2D& a_Texture )
	{
		return ( uint8_t*& )a_Texture.m_Data;
	}

	Directory m_Source;
	Directory m_Output;
};