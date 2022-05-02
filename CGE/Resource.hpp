#pragma once
#include <vector>

#include "entt/resource/loader.hpp"
#include "entt/resource/cache.hpp"
#include "entt/resource/handle.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "STBI.hpp"

#include "Texture.hpp"

class Resource
{
private:

	template < typename T >
	using Cache = entt::resource_cache< T >;

	template < typename T >
	inline static Cache< T > GetCache()
	{
		static Cache< T > Cache;
		return Cache;
	}
	
	static void Initialize()
	{
		// Search for all resources, and compile into a 
	}

public:

	
};

class Texture
{
public:

	Texture( const char* file )
	{
		data = stbi_load( file, &width, &height, &channels, 3 );
	}

	~Texture()
	{
		stbi_image_free( data );
	}

	int width, height, channels;
	stbi_uc* data;
};

class TextureLoader : public entt::resource_loader< TextureLoader, Texture >
{
public:

	entt::resource_handle< Texture > load( const char* file ) const
	{
		return std::make_shared< Texture >( file );
	}
};

class TextureCache : public entt::resource_cache< Texture >
{

};