#pragma once
#include "File.hpp"
#include "Serialization.hpp"

class ResourcePackageHeader
{
private:

	friend class Serialization;
	friend class ResourcePackage;

	size_t GetResourceLocation( const std::string& a_Name )
	{
		auto Iter = m_ResourceLocations.find( a_Name );

		if ( Iter == m_ResourceLocations.end() )
		{
			return size_t( -1 );
		}

		return Iter->second;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> m_ResourceLocations;
	}

	std::map< std::string, size_t > m_ResourceLocations;
};

class ResourcePackage
{
public:

	ResourcePackage( const File& a_File )
		: m_File( a_File )
	{
		_ASSERT_EXPR( m_File.Open(), "Can not open package file." );
		FileDeserializer HeaderDeserializer( m_File );
		HeaderDeserializer >> m_Header;
		m_File.Close();
	}

	template < typename T >
	bool Load( T& o_Resource, const std::string& a_Name )
	{
		size_t Location = m_Header.GetResourceLocation( a_Name );

		if ( Location == size_t( -1 ) )
		{
			return false;
		}

		m_File.Open();
		m_File.Seek( Location );
		FileDeserializer ResourceDeserializer( m_File );
		std::string TypeString;
		ResourceDeserializer >> TypeString;

		if ( TypeString != typeid( T ).name() )
		{
			m_File.Close();
			return false;
		}

		ResourceDeserializer >> o_Resource;
		m_File.Close();
		return true;
	}

	size_t Size() const
	{
		m_Header.m_ResourceLocations.size();
	}

private:

	ResourcePackageHeader m_Header;
	File                  m_File;
};