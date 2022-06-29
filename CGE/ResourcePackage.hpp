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

		return std::get< 1 >( Iter->second );
	}
	
	const std::string& GetName( Hash a_Name ) const
	{
		static std::string EmptyString;

		auto Iter = m_ResourceLocations.find( a_Name );

		if ( Iter != m_ResourceLocations.end() )
		{
			return std::get< 0 >( Iter->second );
		}

		return EmptyString;
	}

	const std::string& GetType( Hash a_Name ) const
	{
		static std::string EmptyString;

		auto Iter = m_ResourceLocations.find( a_Name );

		if ( Iter != m_ResourceLocations.end() )
		{
			return std::get< 2 >( Iter->second );
		}

		return EmptyString;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		std::vector< std::tuple< Name, size_t, std::string > > ResourceLocations;
		a_Deserializer >> ResourceLocations;

		for ( auto Begin = ResourceLocations.begin(), End = ResourceLocations.end(); Begin != End; ++Begin )
		{
			auto& Entry = m_ResourceLocations[ std::get< 0 >( *Begin ).HashCode() ];
			std::get< 0 >( Entry ) = std::string( std::get< 0 >( *Begin ).Data(), std::get< 0 >( *Begin ).Length() );
			std::get< 1 >( Entry ) = std::get< 1 >( *Begin );
			std::get< 2 >( Entry ) = std::get< 2 >( *Begin );
		}
	}

	std::map< Hash, std::tuple< std::string, size_t, std::string > > m_ResourceLocations;
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

		auto& Type = m_Header.GetType( a_Name );

		if ( typeid( T ).name() != Type )
		{
			return false;
		}

		File ResourceFile( m_File.c_str() );

		ResourceFile.Open();
		ResourceFile.Seek( Location );
		FileDeserializer ResourceDeserializer( ResourceFile );
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