#pragma once
#include <string>
#include "File.hpp"
#include "Name.hpp"

#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Texture2D;
class Mesh;
class Material;

class ResourcePackager
{
public:

	bool BuildPackage( const Directory& a_OutputDirectory ) const;

private:

	template < typename T >
	size_t CreateTemp( const Name& a_Name, const std::string& a_TempFile, const Directory& a_TempDirectory, File& a_From ) const
	{
		T Resource;

		if ( !Load( Resource, a_From ) )
		{
			return 0;
		}

		Resource.m_Name = a_Name;

		if ( a_TempDirectory.ContainsFile( a_TempFile.c_str() ) )
		{
			a_TempDirectory.DeleteFile( a_TempFile.c_str() );
		}

		std::string TypeString( typeid( T ).name() );
		size_t ResourceSize = Serialization::GetSizeOf( TypeString );
		ResourceSize += Serialization::GetSizeOf( Resource );
		auto OutputFile = a_TempDirectory.CreateFile( a_TempFile.c_str(), ResourceSize );

		if ( !OutputFile.Open() )
		{
			return 0;
		}

		FileSerializer OutputStream( OutputFile );
		OutputStream << TypeString << Resource;
		OutputFile.Close();
		return ResourceSize;
	}

	bool Load( Texture2D& o_Texture, File& a_File ) const;
	bool Load( Mesh& o_Mesh, File& a_File ) const;
	bool Load( Material& o_Material, File& a_File ) const;
};