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

		if      ( FileType == "texture2d"  ) ResourcesSize += CreateTemp< Texture2D  >( TempFile, TempDirectory, ResourceFile );
		else if ( FileType == "mesh"       ) ResourcesSize += CreateTemp< Mesh       >( TempFile, TempDirectory, ResourceFile );
		else if ( FileType == "material"   ) ResourcesSize += CreateTemp< Material   >( TempFile, TempDirectory, ResourceFile );

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

bool ResourcePackager::Load( Texture2D& o_Texture, File& a_File ) const
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
		
		o_Mesh.m_Vertices.resize( Shapes[ 0 ].mesh.indices.size() );
		for ( size_t i = 0; i < Shapes[ 0 ].mesh.indices.size(); ++i )
		{
			o_Mesh.m_Vertices[ i ] = {
				Shapes[ 0 ].mesh.indices[ i ].texcoord_index,
				Shapes[ 0 ].mesh.indices[ i ].vertex_index,
				Shapes[ 0 ].mesh.indices[ i ].normal_index,
				0,
				0,
				Shapes[ 0 ].mesh.indices[ i ].texcoord_index
			};
		}

		o_Mesh.m_Colours.resize( Attributes.colors.size() / 3 );
		for ( size_t i = 0; i < Attributes.colors.size(); i += 3 )
		{
			o_Mesh.m_Colours[ i / 3 ] = {
				Attributes.colors[ i + 0 ],
				Attributes.colors[ i + 1 ],
				Attributes.colors[ i + 2 ],
				1.0f
			};
		}

		o_Mesh.m_Positions.resize( Attributes.vertices.size() / 3 );
		for ( size_t i = 0; i < Attributes.vertices.size(); i += 3 )
		{
			o_Mesh.m_Positions[ i / 3 ] = { 
				Attributes.vertices[ i + 0 ], 
				Attributes.vertices[ i + 1 ], 
				Attributes.vertices[ i + 2 ] 
			};
		}

		o_Mesh.m_Normals.resize( Attributes.normals.size() / 3 );
		for ( size_t i = 0; i < Attributes.normals.size(); i += 3 )
		{
			o_Mesh.m_Normals[ i / 3 ] = {
				Attributes.normals[ i + 0 ],
				Attributes.normals[ i + 1 ],
				Attributes.normals[ i + 2 ]
			};
		}

		o_Mesh.m_Texels.resize( Attributes.texcoords.size() / 2 );
		for ( size_t i = 0; i < Attributes.texcoords.size(); i += 2 )
		{
			o_Mesh.m_Texels[ i / 2 ] = {
				Attributes.texcoords[ i + 0 ],
				Attributes.texcoords[ i + 1 ]
			};
		}

		if ( o_Mesh.m_Normals.size() > 0 && o_Mesh.m_Texels.size() > 0 )
		{
			Vector3* Tangents = new Vector3[ o_Mesh.m_Vertices.size() * 2 ];
			Vector3* Bitangents = Tangents + o_Mesh.m_Vertices.size();
			memset( Tangents, 0, sizeof( Vector3 ) * o_Mesh.m_Vertices.size() * 2 );

			for ( size_t i = 0; i < o_Mesh.m_Vertices.size(); i += 3 )
			{
				auto* Vertices = o_Mesh.m_Vertices.data() + i;
				auto& P0 = o_Mesh.m_Positions[ Vertices[ 0 ][ 1 ] ];
				auto& P1 = o_Mesh.m_Positions[ Vertices[ 1 ][ 1 ] ];
				auto& P2 = o_Mesh.m_Positions[ Vertices[ 2 ][ 1 ] ];
				auto& V0 = o_Mesh.m_Texels[ Vertices[ 0 ][ 5 ] ];
				auto& V1 = o_Mesh.m_Texels[ Vertices[ 1 ][ 5 ] ];
				auto& V2 = o_Mesh.m_Texels[ Vertices[ 2 ][ 5 ] ];
				auto PA = P1 - P0;
				auto PB = P2 - P0;
				auto VA = V1 - V0;
				auto VB = V2 - V0;
				float R = 1.0f / ( VA.x * VB.y - VA.y * VB.x );

				Vector3 SDir( ( VB.y * PA.x - VA.y * PB.x ) * R, ( VB.y * PA.y - VA.y * PB.y ) * R, ( VB.y * PA.z - VA.y * PB.z ) * R );
				Vector3 TDir( ( VA.x * PB.x - VB.x * PA.x ) * R, ( VA.x * PB.y - VB.x * PA.y ) * R, ( VA.x * PB.z - VB.x * PA.z ) * R );

				Tangents[ i + 0 ] += SDir;
				Tangents[ i + 1 ] += SDir;
				Tangents[ i + 2 ] += SDir;
				Bitangents[ i + 0 ] += TDir;
				Bitangents[ i + 1 ] += TDir;
				Bitangents[ i + 2 ] += TDir;
			}

			o_Mesh.m_Tangents.resize( o_Mesh.m_Vertices.size() );
			o_Mesh.m_Bitangents.resize( o_Mesh.m_Vertices.size() );

			for ( uint32_t i = 0; i < o_Mesh.m_Vertices.size(); ++i )
			{
				auto&    Vertex  = o_Mesh.m_Vertices[ i ];
				Vector3& Normal  = o_Mesh.m_Normals[ Vertex[ 2 ] ];
				Vector3& Tangent = Tangents[ i ];

				Vertex[ 3 ] = i;
				Vertex[ 4 ] = i;

				o_Mesh.m_Tangents  [ i ] = Math::Normalize( Tangent - Normal * Math::Dot( Normal, Tangent ) );
				o_Mesh.m_Bitangents[ i ] = Math::Cross( Normal, Math::Normalize( Tangent ) ) * ( ( Math::Dot( Math::Cross( Normal, Tangent ), Bitangents[ i ] ) < 0.0f ) ? 1.0f : -1.0f );
			}

			delete[] Tangents;
		}
	}

	return true;
}

bool ResourcePackager::Load( Material& o_Material, File& a_File ) const
{
	return true;
}