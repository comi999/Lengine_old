#pragma once
#include "File.hpp"
#include "Name.hpp"
#include "Serialization.hpp"

class ResourcePackageHeader
{
private:

	friend class Serialization;
	friend class ResourcePackage;

	size_t GetResourceLocation( Hash a_Name )
	{
		auto Iter = m_ResourceLocations.find( a_Name );

		if ( Iter == m_ResourceLocations.end() )
		{
			return size_t( -1 );
		}

		return Iter->second.second;
	}
	
	const std::string& GetName( Hash a_Name ) const
	{
		auto Iter = m_ResourceLocations.find( a_Name );

		if ( Iter != m_ResourceLocations.end() )
		{
			return Iter->second.first;
		}
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		std::vector< std::pair< Name, size_t > > ResourceLocations;
		a_Deserializer >> ResourceLocations;

		for ( auto Begin = ResourceLocations.begin(), End = ResourceLocations.end(); Begin != End; ++Begin )
		{
			auto& Pair = m_ResourceLocations[ Begin->first.HashCode() ];
			Pair.first = std::string( Begin->first.Data(), Begin->first.Length() );
			Pair.second = Begin->second;
		}
	}

	std::map< Hash, std::pair< std::string, size_t > > m_ResourceLocations;
};

class ResourcePackage
{
public:

	ResourcePackage() = default;

	void Init( const std::string& a_Path )
	{
		m_File = a_Path;
		File ResourceFile( m_File.c_str() );
		_ASSERT_EXPR( ResourceFile.Open(), "Can not open package file." );
		FileDeserializer HeaderDeserializer( ResourceFile );
		HeaderDeserializer >> m_Header;
		ResourceFile.Close();
	}

	template < typename T >
	bool Load( T& o_Resource, Hash a_Name )
	{
		size_t Location = m_Header.GetResourceLocation( a_Name );

		if ( Location == size_t( -1 ) )
		{
			return false;
		}

		File ResourceFile( m_File.c_str() );

		ResourceFile.Open();
		ResourceFile.Seek( Location );
		FileDeserializer ResourceDeserializer( ResourceFile );
		std::string TypeString;
		ResourceDeserializer >> TypeString;

		if ( TypeString != typeid( T ).name() )
		{
			ResourceFile.Close();
			return false;
		}

		ResourceDeserializer >> o_Resource;
		ResourceFile.Close();
		return true;
	}

	const std::string& GetNameFromHash( Hash a_Name )
	{
		return m_Header.GetName( a_Name );
	}

	size_t Size() const
	{
		m_Header.m_ResourceLocations.size();
	}

private:

	ResourcePackageHeader m_Header;
	std::string           m_File;
};