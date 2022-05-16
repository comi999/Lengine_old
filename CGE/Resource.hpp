#pragma once
#include <vector>
#include <map>
#include "entt/resource/loader.hpp"
#include "entt/resource/cache.hpp"
#include "entt/resource/handle.hpp"
#include "entt/core/hashed_string.hpp"
#include "File.hpp"
#include "Hash.hpp"

#include "Mesh.hpp"
#include "Texture.hpp"

template < typename T >
class ResourceCache : public entt::resource_cache< T > {};

template < typename T >
using ResourceHandle = entt::resource_handle< T >;

class TextureLoader : public entt::resource_loader< TextureLoader, Texture >
{
public:
	ResourceHandle< Texture > load() const;
	ResourceHandle< Texture > load( Vector2Int a_Size ) const;
	ResourceHandle< Texture > load( Vector2Int a_Size, Colour a_Colour ) const;
	ResourceHandle< Texture > load( Colour a_Colour ) const;
	ResourceHandle< Texture > load( const File& a_File ) const; 
};

class MeshLoader : public entt::resource_loader< MeshLoader, Mesh >
{
public:
	ResourceHandle< Mesh > load( const File& a_File ) const;
};

template < typename T > class ResourceLoaderImpl { public: using Type = void;          };
template <> class ResourceLoaderImpl< Texture >  { public: using Type = TextureLoader; };
template <> class ResourceLoaderImpl< Mesh    >  { public: using Type = MeshLoader;    };

template < typename T >
using ResourceLoader = typename ResourceLoaderImpl< T >::Type;

class Resource
{
private:

	static size_t GetNextResourceIndex()
	{
		static size_t Index = 0;
		return Index++;
	}

	template < typename T >
	static size_t GetResourceTypeIndex()
	{
		static size_t Index = GetNextResourceIndex();
		return Index;
	}

	template < typename T >
	static std::map< std::string, File >& Assure()
	{
		static size_t Index = GetResourceTypeIndex< T >();

		if ( !( s_ResourceFileLookup.size() < Index ) )
		{
			s_ResourceFileLookup.resize( Index + 1u );
		}

		return s_ResourceFileLookup[ Index ];
	}

	static std::map< std::string, File >& Assure( size_t a_Index )
	{
		if ( !( a_Index < s_ResourceFileLookup.size() ) )
		{
			s_ResourceFileLookup.resize( a_Index + 1u );
		}

		return s_ResourceFileLookup[ a_Index ];
	}
	
	static void Initialize()
	{
		/*FileRecursiveIterator Begin( GetRootDirectory() );

		while ( Begin )
		{
			File FoundFile( *Begin );
			auto FoundExtension = s_SupportedTypes.find( FoundFile.GetExtension() );

			if ( FoundExtension != s_SupportedTypes.end() )
			{
				size_t TypeIndex = FoundExtension->second;
				auto& Lookup = Assure( TypeIndex );
				Lookup.emplace( FoundFile.GetPath(), FoundFile );
			}

			++Begin;
		}*/
	}

public:

	template < typename T >
	static auto GetResourceByPath( const std::string& a_Path )
	{
		//// Look up in cache to see if already loaded.
		//auto& ResourceCache = GetResourceCache< T >();
		//Hash Hashed = CRC32_RT( a_Path.c_str() );
		//
		//if ( ResourceCache.contains( Hashed ) )
		//{
		//	return ResourceCache.handle( Hashed );
		//}

		//// If not loaded, look in Resources folder.
		//static size_t Index = GetResourceTypeIndex< T >();

		//// If no type was found in Resources matching T, abort.
		//if ( !( Index < s_ResourceFileLookup.size() ) )
		//{
		//	return entt::resource_handle< T >();
		//}

		//auto& Lookup = s_ResourceFileLookup[ Index ];
		//auto FileIt = Lookup.find( Path( a_Path ) );
		//
		//// If no file found, return nullptr.
		//if ( FileIt == Lookup.end() )
		//{
		//	return entt::resource_handle< T >();
		//}

		//return ResourceCache.load< ResourceLoader< T > >( Hashed, FileIt->second );
	}

	static const Directory& GetRootDirectory()
	{
		return s_ResourceDirectory;
	}

private:

	template < typename T >
	static ResourceCache< T >& GetResourceCache()
	{
		static ResourceCache< T > Cache;
		return Cache;
	}

	template < typename T >
	static ResourceLoader< T >& GetResourceLoader()
	{
		static typename ResourceLoader< T >::Type ResourceLoader;
		return ResourceLoader;
	}

	friend class CGE;

	static Directory                                    s_ResourceDirectory;
	static std::map< std::string, size_t >              s_SupportedTypes;
	static std::vector< std::map< std::string, File > > s_ResourceFileLookup;
};