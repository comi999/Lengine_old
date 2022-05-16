#include "OFBX.hpp"
#include "Resource.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "STBI.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "TOBJ.hpp"

ResourceHandle< Texture > TextureLoader::load() const
{
	return std::make_shared< Texture >();
}

ResourceHandle< Texture > TextureLoader::load( Vector2Int a_Size ) const
{
	return std::make_shared< Texture >( a_Size );
}

ResourceHandle< Texture > TextureLoader::load( Vector2Int a_Size, Colour a_Colour ) const
{
	return std::make_shared< Texture >( a_Size, a_Colour );
}

ResourceHandle< Texture > TextureLoader::load( Colour a_Colour ) const
{
	return std::make_shared< Texture >( a_Colour );
}

ResourceHandle< Texture > TextureLoader::load( const File& a_File ) const
{
	return std::make_shared< Texture >( a_File );
}

Directory Resource::s_ResourceDirectory( "./Resources" );
std::map< std::string, size_t > Resource::s_SupportedTypes
{
	{ ".bmp", Resource::GetResourceTypeIndex< Texture >() },
	{ ".png", Resource::GetResourceTypeIndex< Texture >() },
	{ ".jpg", Resource::GetResourceTypeIndex< Texture >() },
	{ ".obj", Resource::GetResourceTypeIndex< Mesh >() }
};
std::vector< std::map< std::string, File > > Resource::s_ResourceFileLookup;