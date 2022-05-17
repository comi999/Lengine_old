#include "ResourcePackager.hpp"
#include "File.hpp"
#include "JSON.hpp"
#include "OFBX.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "STBI.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "TOBJ.hpp"

// Resource types
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

bool ResourcePackager::BuildPackage( const Directory& a_OutputDirectory ) const
{
	Directory ThisDir;

	if ( !ThisDir.ContainsFile( "manifest.json" ) )
	{
		return false;
	}

	File ManifestFile = ThisDir.GetFile( "manifest.json" );
	ManifestFile.Open();

	if ( !ManifestFile.IsOpen() )
	{
		return false;
	}

	Json Manifest;
	std::ifstream ManifestStream = ManifestFile;
	ManifestStream >> Manifest;
	ManifestFile.Close();
	Json Resources = Manifest[ "Resources" ];
	Directory TempDirectory =
		ThisDir.ContainsDirectory( "Temp" ) ?
		ThisDir.GetDirectory( "Temp" ) :
		ThisDir.CreateDirectory( "Temp" );

	std::vector< std::pair< std::string, size_t > > ResourceHeader;
	std::vector< File > ResourceFiles;


	size_t HeaderSize = 0;
	size_t ResourcesSize = 0;

	for ( auto Begin = Resources.begin(), End = Resources.end(); Begin != End; ++Begin )
	{
		std::string Key = Begin.key();
		Json Value = *Begin;

		if ( !( Value.contains( "Path" ) && Value.contains( "Type" ) ) )
		{
			return false;
		}

		std::string FilePath = Value[ "Path" ].get< std::string >();
		std::string FileType = Value[ "Type" ].get< std::string >();

		File ResourceFile( FilePath.c_str() );

		ResourceHeader.emplace_back( Key, ResourcesSize );

		HeaderSize += Serialization::GetSizeOf( Key );
		HeaderSize += sizeof( size_t );

		std::string TempFile = Key + "." + FileType;

		if      ( FileType == "texture"  ) ResourcesSize += CreateTemp< Texture  >( TempFile, TempDirectory, ResourceFile );
		else if ( FileType == "mesh"     ) ResourcesSize += CreateTemp< Mesh     >( TempFile, TempDirectory, ResourceFile );
		else if ( FileType == "material" ) ResourcesSize += CreateTemp< Material >( TempFile, TempDirectory, ResourceFile );

		ResourceFiles.push_back( TempDirectory.GetFile( TempFile.c_str() ) );
	}

	// Compile into a Resource Package.
	if ( a_OutputDirectory.ContainsFile( "resource.package" ) )
	{
		a_OutputDirectory.DeleteFile( "resource.package" );
	}

	File PackageFile = a_OutputDirectory.CreateFile( "resource.package", HeaderSize + ResourcesSize );
	
	if ( !PackageFile.Open() )
	{
		return false;
	}

	FileSerializer PackageSerializer( PackageFile );
	PackageSerializer << ResourceHeader.size();
	HeaderSize += sizeof( ResourceHeader.size() );

	for ( auto Begin = ResourceHeader.begin(), End = ResourceHeader.end(); Begin != End; ++Begin )
	{
		PackageSerializer << Begin->first;
		PackageSerializer << Begin->second + HeaderSize;
	}

	for ( auto Begin = ResourceFiles.begin(), End = ResourceFiles.end(); Begin != End; ++Begin )
	{
		File& ResourceFile = *Begin;
		
		if ( !ResourceFile.Open() )
		{
			return false;
		}
		
		BufferStream Buffer( ResourceFile.Size() );
		ResourceFile.Read( Buffer.Data(), Buffer.Size() );
		ResourceFile.Close();
		PackageFile.Write( Buffer.Data(), Buffer.Size() );
	}

	PackageFile.Close();
	return true;
}

bool ResourcePackager::Load( Texture& o_Texture, File& a_File ) const
{
	std::string FilePath = a_File;
	o_Texture.m_Data = ( Colour* )stbi_load( FilePath.c_str(), &o_Texture.m_Size.x, &o_Texture.m_Size.y, nullptr, 4 );
	return o_Texture.m_Data;
}

bool ResourcePackager::Load( Mesh& o_Mesh, File& a_File ) const
{
	if ( a_File.GetExtension() == ".obj" )
	{
		tinyobj::attrib_t Attributes;
		std::vector< tinyobj::shape_t > Shapes;
		std::string Warning;
		std::string Error;
		a_File.Open();
		std::ifstream FileStream = a_File;
		bool Success = tinyobj::LoadObj( &Attributes, &Shapes, nullptr, &Warning, &Error, &FileStream );
		
		for ( size_t i = 0; i < Shapes[ 0 ].mesh.indices.size(); ++i )
		{
			o_Mesh.m_Vertices.emplace_back(
				Shapes[ 0 ].mesh.indices[ i ].texcoord_index,
				Shapes[ 0 ].mesh.indices[ i ].vertex_index,
				Shapes[ 0 ].mesh.indices[ i ].normal_index,
				Shapes[ 0 ].mesh.indices[ i ].texcoord_index
			);
		}

		for ( size_t i = 0; i < Attributes.colors.size(); i += 3 )
		{
			o_Mesh.m_Colours.emplace_back(
				255 * Attributes.colors[ i + 0 ],
				255 * Attributes.colors[ i + 1 ],
				255 * Attributes.colors[ i + 2 ]
			);
		}

		for ( size_t i = 0; i < Attributes.vertices.size(); i += 3 )
		{
			o_Mesh.m_Positions.emplace_back( 
				Attributes.vertices[ i + 0 ], 
				Attributes.vertices[ i + 1 ], 
				Attributes.vertices[ i + 2 ] 
			);
		}

		for ( size_t i = 0; i < Attributes.normals.size(); i += 3 )
		{
			o_Mesh.m_Normals.emplace_back(
				Attributes.normals[ i + 0 ],
				Attributes.normals[ i + 1 ],
				Attributes.normals[ i + 2 ]
			);
		}

		for ( size_t i = 0; i < Attributes.texcoords.size(); i += 2 )
		{
			o_Mesh.m_Texels.emplace_back(
				Attributes.texcoords[ i + 0 ],
				Attributes.texcoords[ i + 1 ]
			);
		}
	}

	return true;
}

bool ResourcePackager::Load( Material& o_Material, File& a_File ) const
{
	return true;
}