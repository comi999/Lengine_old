#include "ResourcePackager.hpp"
#include "File.hpp"
#include "JSON.hpp"
#include "OFBX.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "STBI.hpp"

//#define TINYOBJLOADER_IMPLEMENTATION
//#include "TOBJ.hpp"


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

	//std::vector< std::pair< std::string, size_t > > ResourceHeader;
	std::vector< std::pair< Name, size_t > > ResourceHeader;
	std::vector< File > ResourceFiles;

	size_t HeaderSize = 0;
	size_t ResourcesSize = 0;

	for ( auto Begin = Resources.begin(), End = Resources.end(); Begin != End; ++Begin )
	{
		//std::string Key = Begin.key();
		Name Key = Begin.key();
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

		std::string KeyName = Key;
		std::string TempFile = KeyName + "." + FileType;

		if      ( FileType == "texture2d"  ) ResourcesSize += CreateTemp< Texture2D  >( Key, TempFile, TempDirectory, ResourceFile );
		else if ( FileType == "mesh"       ) ResourcesSize += CreateTemp< Mesh       >( Key, TempFile, TempDirectory, ResourceFile );
		else if ( FileType == "material"   ) ResourcesSize += CreateTemp< Material   >( Key, TempFile, TempDirectory, ResourceFile );

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
		Assimp::Importer Importer;
		auto Scene = Importer.ReadFile( a_File, aiPostProcessSteps::aiProcess_CalcTangentSpace | aiPostProcessSteps::aiProcess_Triangulate | aiPostProcessSteps::aiProcess_JoinIdenticalVertices );
		
		if ( !Scene->HasMeshes() )
		{
			return false;
		}

		auto Mesh = Scene->mMeshes[ 0 ];

		if ( Mesh->HasFaces() )
		{
			uint32_t IndexCount = Mesh->mNumFaces * 3u;
			o_Mesh.m_Indices.resize( IndexCount );
			aiFace* Face = Mesh->mFaces;

			for ( uint32_t i = 0, *Begin = o_Mesh.m_Indices.data(), *End = Begin + IndexCount; Begin != End; ++i, Begin += 3, ++Face )
			{
				Begin[ 0 ] = Face->mIndices[ 0 ];
				Begin[ 1 ] = Face->mIndices[ 1 ];
				Begin[ 2 ] = Face->mIndices[ 2 ];
			}
		}

		if ( Mesh->HasVertexColors( 0 ) )
		{
			o_Mesh.m_Colours.resize( Mesh->mNumVertices );

			for ( uint32_t i = 0; i < Mesh->mNumVertices; ++i )
			{
				o_Mesh.m_Colours[ i ] = {
					Mesh->mColors[ 0 ][ i ].r,
					Mesh->mColors[ 0 ][ i ].g,
					Mesh->mColors[ 0 ][ i ].b,
					Mesh->mColors[ 0 ][ i ].a 
				};
			};
		}

		if ( Mesh->HasPositions() )
		{
			o_Mesh.m_Positions.resize( Mesh->mNumVertices );

			for ( uint32_t i = 0; i < Mesh->mNumVertices; ++i )
			{
				o_Mesh.m_Positions[ i ] = {
					Mesh->mVertices[ i ][ 0 ],
					Mesh->mVertices[ i ][ 1 ],
					Mesh->mVertices[ i ][ 2 ]
				};
			}
		}

		if ( Mesh->HasNormals() )
		{
			o_Mesh.m_Normals.resize( Mesh->mNumVertices );

			for ( uint32_t i = 0; i < Mesh->mNumVertices; ++i )
			{
				o_Mesh.m_Normals[ i ] = {
					Mesh->mNormals[ i ][ 0 ],
					Mesh->mNormals[ i ][ 1 ],
					Mesh->mNormals[ i ][ 2 ]
				};
			}
		}

		if ( Mesh->HasTangentsAndBitangents() )
		{
			o_Mesh.m_Tangents.resize( Mesh->mNumVertices );
			o_Mesh.m_Bitangents.resize( Mesh->mNumVertices );

			for ( uint32_t i = 0; i < Mesh->mNumVertices; ++i )
			{
				o_Mesh.m_Tangents[ i ] = {
					Mesh->mTangents[ i ][ 0 ],
					Mesh->mTangents[ i ][ 1 ],
					Mesh->mTangents[ i ][ 2 ]
				};

				o_Mesh.m_Bitangents[ i ] = {
					Mesh->mBitangents[ i ][ 0 ],
					Mesh->mBitangents[ i ][ 1 ],
					Mesh->mBitangents[ i ][ 2 ]
				};
			}
		}

		if ( Mesh->HasTextureCoords( 0 ) )
		{
			o_Mesh.m_Texels.resize( Mesh->mNumVertices );

			for ( uint32_t i = 0; i < Mesh->mNumVertices; ++i )
			{
				o_Mesh.m_Texels[ i ] = {
					Mesh->mTextureCoords[ 0 ][ i ][ 0 ],
					Mesh->mTextureCoords[ 0 ][ i ][ 1 ]
				};
			}
		}
	}

	return true;
}

bool ResourcePackager::Load( Material& o_Material, File& a_File ) const
{
	Assimp::Importer Importer;
	auto Scene = Importer.ReadFile( a_File, 0 );
	
	if ( !Scene->HasMaterials() )
	{
		return false;
	}

	auto Material = Scene->mMaterials[ 1 ];

	//Material->
	return true;
}