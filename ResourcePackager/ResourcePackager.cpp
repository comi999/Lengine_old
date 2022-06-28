#include "ResourcePackager.hpp"
#include "File.hpp"
//#include "OFBX.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "STBI.hpp"
//#define TINYOBJLOADER_IMPLEMENTATION
//#include "TOBJ.hpp"


// Resource types
#include "Prefab.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

// Component types
#include "Transform.hpp"
#include "MeshRenderer.hpp"
#include "Alias.hpp"


// There is always a root node.
// 
// first load all materials, textures, meshes.
// name them via the root name. If they don't have a name themselves, give them
// a name. eg. "assetname_material0", "assetname_material1"
// If it does have a name, use it instead. eg. "assetname_diffuse_texture", "assetname_lilly_texture"
// These should be pushed to a queue to load into raw resource with a name/hash"
// 
// Materials should reference the textures in the resource.package via their hash
// Scenes should also be created that store the relationships between objects, their desired materials
// etc.

// When a material is loaded from Resource, it should automatically load all dependencies.
// When a scene is loaded, it should load all meshes and materials that are dependent on it, and create gameobjects
// that store the relationships between them.

// In the manifest.json, you should be able to load a scene, material, mesh, texture,
// If a resource contains multiple of the above types, you should be able to specify an index

// eg.
// "Resources": {
//     "store_front": {
//         "Type": "prefab",
//         "Path": "./SomeFolder/storefront.fbx"
//		   "Objects": {
//             "table_mesh": {
//                 "Type": "mesh",
//                 "Index": 1
//             },
//             "table_diffuse": {
//                  "Type": "texture",
//                  "Index": 2
//             }
//         }
//     },
//     "grass_mesh": {
//         "Path": "./SomeFolder/grass.fbx",
//         "Type": "mesh",
//         "Index": 2
//     },
//     "picture_frame": {
//         "Type": "texture",
//         "Path": "./SomeFolder/picture_frame.png"
//     }
// }

// This gets processed into this list of LoadInstructions:
// 0:
//   - File: "./SomeFolder/storefront.fbx"
//   - Type: "prefab"
//   - Name: "store_front"
//   - Index: 0
// 1:
//   - File: "./SomeFolder/storefront.fbx"
//   - Type: "mesh"
//   - Name: "table_mesh"
//   - Index: 1
// 2:
//   - File: "./SomeFolder/storefront.fbx"
//   - Type: "texture"
//   - Name: "table_diffuse"
//   - Index: 2
// 3:
//   - File: "./SomeFolder/grass.fbx"
//   - Type: "mesh"
//   - Name: "grass_mesh"
//   - Index: 2
// 4:
//   - File: "./SomeFolder/picture_frame.png"
//   - Type: "texture2d"
//   - Name: "picture_frame"
//   - Index: 0

bool ResourcePackager::BuildPackage( const Directory& a_OutputDirectory )
{
	if ( !m_SourceDirectory.ContainsFile( "manifest.json" ) )
	{
		// Logger::Error( "No manifest.json file found in %s.", ThisDirectory );
		return false;
	}

	File ManifestFile = m_SourceDirectory.GetFile( "manifest.json" );

	if ( !ManifestFile.Open() )
	{
		// Logger::Error( "manifest.json file could not be opened." );
		return false;
	}

	Json Manifest;
	std::ifstream ManifestStream = ManifestFile;
	ManifestStream >> Manifest;
	ManifestFile.Close();

	if ( m_SourceDirectory.ContainsDirectory( "Temp" ) )
	{
		m_SourceDirectory.DestroyDirectory( "Temp" );
	}

	m_TempDirectory = m_SourceDirectory.NewDirectory( "Temp" );

	if ( Manifest.find( "Resources" ) == Manifest.end() )
	{
		// Logger::Error( "No Resources object found in manifest.json" );
		return false;
	}

	const Json& Resources = Manifest[ "Resources" ];

	for ( auto Begin = Resources.begin(), End = Resources.end(); Begin != End; ++Begin )
	{
		const std::string& Key = Begin.key();

		if ( !ProcessResourceEntry( Key, *Begin ) )
		{
			// Logger::Error( "Failed to load resource entry: %s", Key );
		}
	}
	
	// Sort the load instructions.
	m_LoadInstructions.sort();

	while ( !m_LoadInstructions.empty() )
	{
		auto Begin = m_LoadInstructions.begin();
		auto End = Begin;
		while ( End != m_LoadInstructions.end() && Begin->File == End->File ) ++End;
		Path FilePath( Begin->File.c_str() );

		if ( !FilePath.IsFile() )
		{
			continue;
		}

		File ResourceFile( FilePath );
		std::string ResourceExtension = ResourceFile.GetExtension();

		std::list< LoadInstruction > Instructions;
		Instructions.splice( Instructions.begin(), m_LoadInstructions, Begin, End );

		static const std::map< std::string, bool(*)( const Directory&, const Directory&, std::list< LoadInstruction >& ) > ExtensionMap {
			std::make_pair( ".obj",  CreateTempResources_Assimp ),
			std::make_pair( ".fbx",  CreateTempResources_Assimp ),
			std::make_pair( ".bmp",  CreateTempResources_STBI   ),
			std::make_pair( ".jpg",  CreateTempResources_STBI   ),
			std::make_pair( ".jpeg", CreateTempResources_STBI   ),
			std::make_pair( ".png",  CreateTempResources_STBI   ),
			std::make_pair( ".tga",  CreateTempResources_STBI   )
			// need to put in all of the supported file extensions.
		};

		auto FoundExtension = ExtensionMap.find( ResourceExtension );

		if ( FoundExtension == ExtensionMap.end() )
		{
			// Logger::Error( "Unsupported type %s found.", FoundExtension );
			continue;
		}

		FoundExtension->second( m_SourceDirectory, m_TempDirectory, Instructions );
	}

	std::vector< std::tuple< Name, size_t, std::string > > ResourceHeader;
	std::vector< File > ResourceFiles;
	size_t ResourcesSize = 0;

	{
		DirectoryIterator Begin( m_TempDirectory );

		for ( ; Begin; ++Begin )
		{
			Path ResourcePath = Begin;

			if ( !ResourcePath.IsFile() )
			{
				continue;
			}

			File ResourceFile( ResourcePath );
			Name ResourceName( ResourceFile.GetStem() );
			auto& NewHeader = ResourceHeader.emplace_back( ResourceName, ResourcesSize, "" );
			std::string ResourceExtension = ResourceFile.GetExtension();

			if ( ResourceExtension == ".mesh" )
			{
				std::get< 2 >( NewHeader ) = typeid( Mesh ).name();
			}
			else if ( ResourceExtension == ".material" )
			{
				std::get< 2 >( NewHeader ) = typeid( Material ).name();
			}
			else if ( ResourceExtension == ".texture2d" )
			{
				std::get< 2 >( NewHeader ) = typeid( Texture2D ).name();
			}
			else if ( ResourceExtension == ".prefab" )
			{
				std::get< 2 >( NewHeader ) = typeid( Prefab ).name();
			}

			ResourceFile.Open();
			ResourcesSize += ResourceFile.Size();
			ResourceFile.Close();
		}
	}

	size_t HeaderSize = Serialization::GetSizeOf( ResourceHeader );

	for ( auto& Entry : ResourceHeader )
	{
		std::get< 1 >( Entry ) += HeaderSize;
	}

	ResourcesSize += HeaderSize;

	if ( m_SourceDirectory.ContainsFile( "resource.package" ) )
	{
		m_SourceDirectory.DestroyFile( "resource.package" );
	}

	File OutputFile = m_SourceDirectory.NewFile( "resource.package", ResourcesSize );
	OutputFile.Open();
	FileSerializer Serializer( OutputFile );
	Serializer << ResourceHeader;

	{
		DirectoryIterator Begin( m_TempDirectory );

		for ( ; Begin; ++Begin )
		{
			Path ResourcePath = Begin;

			if ( !ResourcePath.IsFile() )
			{
				continue;
			}

			File ResourceFile( ResourcePath );
			ResourceFile.Open();
			std::vector< uint8_t > TempBuffer;
			TempBuffer.resize( ResourceFile.Size() );
			ResourceFile.Read( TempBuffer.data(), TempBuffer.size() );
			ResourceFile.Close();
			Serializer.Stream().Write( TempBuffer.data(), TempBuffer.size() );
		}
	}

	OutputFile.Close();

	m_SourceDirectory.DestroyDirectory( "Temp" );
}

bool ResourcePackager::ProcessResourceEntry( const std::string& a_Key, const Json& a_Entry )
{
	auto Type = a_Entry.find( "Type" );

	if ( Type == a_Entry.end() )
	{
		// Logger::Error( "No type specified for %s.", a_Key );
		return false;
	}

	auto Path = a_Entry.find( "Path" );

	if ( Path == a_Entry.end() )
	{
		// Logger::Error( "No path specified for %s.", a_Key );
		return false;
	}

	std::string PathString = Path->get< std::string >();
	if ( !m_SourceDirectory.ContainsFile( PathString.c_str() ) )
	{
		// Logger::Error( "No path specified for %s.", a_Key );
		return false;
	}

	File ResourceFile( PathString );
	auto Index = a_Entry.find( "Index" );

	// Push LoadInstruction.
	LoadInstruction Instruction = { 
		ResourceFile,
		Type->get< std::string >(),
		Name( a_Key ),
		Index != a_Entry.end() ? Index->get< uint32_t >() : 0
	};

	m_LoadInstructions.push_back( Instruction );
	auto Objects = a_Entry.find( "Objects" );

	if ( Objects == a_Entry.end() )
	{
		return true;
	}

	for ( auto Begin = Objects->begin(), End = Objects->end(); Begin != End; ++Begin )
	{
		Type = Begin->find( "Type" );

		if ( Type == Begin->end() )
		{
			// Logger::Error( "No type specified for object %s in %s.", Begin.key(), a_Key );
			continue;
		}

		Index = Begin->find( "Index" );

		if ( Index == Begin->end() )
		{
			// Logger::Error( "No index specified for object %s in %s.", Begin.key(), a_Key );
			continue;
		}

		m_LoadInstructions.back().Names.NamingEntries.emplace_back(
			Type->get< std::string >(),
			Index->get< uint32_t >(),
			Begin.key()
		);
	}

}

bool ResourcePackager::CreateTempResources_Assimp( const Directory& a_SourceDirectory, const Directory& a_TempDirectory, std::list< LoadInstruction >& a_Instructions )
{
	static constexpr auto MaterialHandler = []( const aiScene* a_Scene, uint32_t a_Index, Material& o_Material )
	{
		if ( !( a_Index < a_Scene->mNumMaterials ) )
		{
			// Logger::Error( "Material index out of range. Max index is %i", a_Scene->mNumMaterials - 1 );
			return;
		}

		aiMaterial* Material = a_Scene->mMaterials[ a_Index ];

		aiString StringProperty;
		aiVector3D VectorProperty;
		int32_t IntProperty;
		float FloatProperty;

		if ( Material->Get( AI_MATKEY_NAME, StringProperty ) )
		{
			o_Material.AddProperty( "string_name"_N, std::string( StringProperty.C_Str(), StringProperty.length ) );
		}

		if ( Material->Get( AI_MATKEY_COLOR_DIFFUSE, VectorProperty ) )
		{
			o_Material.AddProperty( "colour_diffuse"_N, reinterpret_cast< Vector3& >( VectorProperty ) );
		}

		if ( Material->Get( AI_MATKEY_COLOR_SPECULAR, VectorProperty ) )
		{
			o_Material.AddProperty( "colour_specular"_N, reinterpret_cast< Vector3& >( VectorProperty ) );
		}

		if ( Material->Get( AI_MATKEY_COLOR_AMBIENT, VectorProperty ) )
		{
			o_Material.AddProperty( "colour_ambient"_N, reinterpret_cast< Vector3& >( VectorProperty ) );
		}

		if ( Material->Get( AI_MATKEY_COLOR_EMISSIVE, VectorProperty ) )
		{
			o_Material.AddProperty( "colour_emissive"_N, reinterpret_cast< Vector3& >( VectorProperty ) );
		}

		if ( Material->Get( AI_MATKEY_COLOR_TRANSPARENT, VectorProperty ) )
		{
			o_Material.AddProperty( "colour_transparent"_N, reinterpret_cast< Vector3& >( VectorProperty ) );
		}

		if ( Material->Get( AI_MATKEY_COLOR_REFLECTIVE, VectorProperty ) )
		{
			o_Material.AddProperty( "colour_reflective"_N, reinterpret_cast< Vector3& >( VectorProperty ) );
		}

		if ( Material->Get( AI_MATKEY_REFLECTIVITY, FloatProperty ) )
		{
			o_Material.AddProperty( "float_reflectivity"_N, FloatProperty );
		}

		if ( Material->Get( AI_MATKEY_SHADING_MODEL, IntProperty ) )
		{
			o_Material.AddProperty( "int_shading_model"_N, IntProperty );
		}

		// Textures
		if ( Material->Get( AI_MATKEY_TEXTURE( aiTextureType::aiTextureType_DIFFUSE, 0 ), StringProperty ) )
		{
			o_Material.AddProperty( "texture_diffuse"_N, std::string( StringProperty.C_Str(), StringProperty.length ) );
		}

		if ( Material->Get( AI_MATKEY_TEXTURE( aiTextureType::aiTextureType_NORMALS, 0 ), StringProperty ) )
		{
			o_Material.AddProperty( "texture_normal"_N, std::string( StringProperty.C_Str(), StringProperty.length ) );
		}

		if ( Material->Get( AI_MATKEY_TEXTURE( aiTextureType::aiTextureType_SPECULAR, 0 ), StringProperty ) )
		{
			o_Material.AddProperty( "texture_specular"_N, std::string( StringProperty.C_Str(), StringProperty.length ) );
		}
		
		// Need to add the rest of the properties that aiMaterial contains.
	};

	static constexpr auto MeshHandler = []( const aiScene* a_Scene, uint32_t a_Index, Mesh& o_Mesh )
	{
		if ( !( a_Index < a_Scene->mNumMeshes ) )
		{
			// Logger::Error( "Mesh index out of range. Max index is %i", a_Scene->mNumMeshes - 1 );
			return;
		}

		aiMesh* Mesh = a_Scene->mMeshes[ a_Index ];

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

		for ( uint32_t i = 0; i < 8; ++i ) // There is 8 possible colour channels
		{
			if ( Mesh->HasVertexColors( i ) )
			{
				o_Mesh.m_Colours[ i ].resize( Mesh->mNumVertices );

				for ( uint32_t j = 0; j < Mesh->mNumVertices; ++j )
				{
					o_Mesh.m_Colours[ i ][ j ] = {
						Mesh->mColors[ i ][ j ].r,
						Mesh->mColors[ i ][ j ].g,
						Mesh->mColors[ i ][ j ].b,
						Mesh->mColors[ i ][ j ].a
					};
				};
			}
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

		for ( uint32_t i = 0; i < 8; ++i ) // There is 8 possible texel channels.
		{
			if ( Mesh->HasTextureCoords( i ) )
			{
				o_Mesh.m_Texels[ i ].resize( Mesh->mNumVertices );

				for ( uint32_t j = 0; j < Mesh->mNumVertices; ++j )
				{
					o_Mesh.m_Texels[ i ][ j ] = {
						Mesh->mTextureCoords[ i ][ j ][ 0 ],
						Mesh->mTextureCoords[ i ][ j ][ 1 ]
					};
				}
			}
		}
	};

	static constexpr auto Texture2DHandler = []( const aiScene* a_Scene, uint32_t a_Index, Texture2D& o_Texture2D )
	{
		if ( !( a_Index < a_Scene->mNumTextures ) )
		{
			// Logger::Error( "Texture index out of range. Max index is %i", a_Scene->mNumTextures - 1 );
			return;
		}

		aiTexture* Texture2D = a_Scene->mTextures[ a_Index ];
		
		if ( !Texture2D->mHeight )
		{
			o_Texture2D.m_Data = ( Colour* )stbi_load_from_memory( 
				reinterpret_cast< uint8_t* >( Texture2D->pcData ), 
				Texture2D->mWidth, 
				&o_Texture2D.m_Size.x, 
				&o_Texture2D.m_Size.y, 
				nullptr, 4 );
		}
		else
		{
			o_Texture2D.m_Data = ( Colour* )Texture2D->pcData;
			o_Texture2D.m_Size = { Texture2D->mWidth, Texture2D->mHeight };
		}
	};

	static constexpr void( *PrefabHandler )( const aiScene*, const aiNode*, const std::vector< Material >&, const std::vector< Mesh >&, const std::vector< Texture2D >&, Prefab& ) = []( const aiScene* a_Scene, const aiNode* a_Node, const std::vector< Material >& a_Materials, const std::vector< Mesh >& a_Meshes, const std::vector< Texture2D >& a_Texture2Ds, Prefab& o_Prefab )
	{
		// Serialize the transform.
		{
			Matrix4 Transformation = *reinterpret_cast< const Matrix4* >( &a_Node->mTransformation.a1 );
			Transform NodeTransform;
			Matrix4::Decompose( 
				Transformation, 
				NodeTransform.m_LocalPosition, 
				NodeTransform.m_LocalRotation, 
				NodeTransform.m_LocalScale );
			auto& Data = o_Prefab.m_Components[ typeid( Transform ).hash_code() ];
			Data.resize( Serialization::GetSizeOf( NodeTransform ) );
			BufferStream Stream;
			Stream.Open( Data.data(), Data.size() );
			BufferSerializer Serializer( Stream );
			Serializer << NodeTransform;
			Stream.Close();
		}

		// Serialize the alias
		{
			Alias NodeAlias;
			Name NodeName = a_Node->mName.length ? Name( ( char* )a_Node->mName.data, a_Node->mName.length ) : Name( "NewGameObject" );
			NodeAlias.SetName( NodeName );
			auto& Data = o_Prefab.m_Components[ typeid( Alias ).hash_code() ];
			Data.resize( Serialization::GetSizeOf( NodeAlias ) );
			BufferStream Stream;
			Stream.Open( Data.data(), Data.size() );
			BufferSerializer Serializer( Stream );
			Serializer << NodeAlias;
			Stream.Close();
		}

		// Serialize other components...
		// Need to add support for more than one mesh per node, but for now, just use first index.
		if ( a_Node->mNumMeshes )
		{
			MeshRenderer NodeMeshRenderer;
			Hash MeshHash = a_Meshes[ a_Node->mMeshes[ 0 ] ].m_Name.HashCode();
			Hash MaterialHash = a_Materials[ a_Scene->mMeshes[ a_Node->mMeshes[ 0 ] ]->mMaterialIndex ].m_Name.HashCode();
			NodeMeshRenderer.m_Material = MaterialHash;
			NodeMeshRenderer.m_Mesh = MeshHash;
			auto& Data = o_Prefab.m_Components[ typeid( MeshRenderer ).hash_code() ];
			Data.resize( Serialization::GetSizeOf( NodeMeshRenderer ) );
			BufferStream Stream;
			Stream.Open( Data.data(), Data.size() );
			BufferSerializer Serializer( Stream );
			Serializer << NodeMeshRenderer;
			Stream.Close();
		}

		for ( uint32_t i = 0; i < a_Node->mNumChildren; ++i )
		{
			auto& Child = o_Prefab.m_Children.emplace_back();
			PrefabHandler( a_Scene, a_Node->mChildren[ i ], a_Materials, a_Meshes, a_Texture2Ds, Child );
		}
	};

	Assimp::Importer Importer;
	const aiScene* Loaded = Importer.ReadFile( 
		a_Instructions.front().File,
	    aiPostProcessSteps::aiProcess_CalcTangentSpace | 
		aiPostProcessSteps::aiProcess_Triangulate | 
		aiPostProcessSteps::aiProcess_JoinIdenticalVertices );

	for ( auto Begin = a_Instructions.begin(), End = a_Instructions.end(); Begin != End; ++Begin )
	{
		if ( Begin->Type == "prefab" )
		{
			Begin->Names.NamingEntries.sort();
			std::list< uint32_t > NamedIndices;
			std::list< NameList::Entry > MaterialNames;
			std::list< NameList::Entry > MeshNames;
			std::list< NameList::Entry > Texture2DNames;
			std::vector< Material > LoadedMaterials;
			std::vector< Mesh > LoadedMeshes;
			std::vector< Texture2D > LoadedTexture2Ds;
			LoadedMaterials.resize( Loaded->mNumMaterials );
			LoadedMeshes.resize( Loaded->mNumMeshes );
			LoadedTexture2Ds.resize( Loaded->mNumTextures );

			for ( auto NameBegin = Begin->Names.NamingEntries.begin(), NameEnd = Begin->Names.NamingEntries.end(); NameBegin != NameEnd; )
			{
				if ( NameBegin->Type == "material" && NameBegin->Index < Loaded->mNumMaterials  )
				{
					auto Iter = NameBegin++;
					MaterialNames.splice( MaterialNames.end(), Begin->Names.NamingEntries, Iter );
				}
				else if ( NameBegin->Type == "mesh" && NameBegin->Index < Loaded->mNumMeshes )
				{
					auto Iter = NameBegin++;
					MeshNames.splice( MeshNames.end(), Begin->Names.NamingEntries, Iter );
				}
				else if ( NameBegin->Type == "texture2d" && NameBegin->Index < Loaded->mNumTextures )
				{
					auto Iter = NameBegin++;
					Texture2DNames.splice( Texture2DNames.end(), Begin->Names.NamingEntries, Iter );
				}
				else
				{
					++NameBegin;
				}
				// and so on...
			}

			for ( uint32_t i = 0; i < Loaded->mNumMaterials; ++i )
			{
				MaterialHandler( Loaded, i, LoadedMaterials[ i ] );
			}

			for ( uint32_t i = 0; i < Loaded->mNumMeshes; ++i )
			{
				MeshHandler( Loaded, i, LoadedMeshes[ i ] );
			}

			for ( uint32_t i = 0; i < Loaded->mNumTextures; ++i )
			{
				Texture2DHandler( Loaded, i, LoadedTexture2Ds[ i ] );
			}
			
			// Name the materials.
			if ( LoadedMaterials.size() ) 
			{
				for ( auto NameBegin = MaterialNames.begin(), NameEnd = MaterialNames.end(); NameBegin != NameEnd; ++NameBegin )
				{
					LoadedMaterials[ NameBegin->Index ].m_Name = Name( NameBegin->Name );
					NamedIndices.push_back( NameBegin->Index );
				}

				uint32_t CurrentUnnamedIndex = 0, CurrentNamedIndex = -1;

				if ( NamedIndices.size() )
				{
					CurrentNamedIndex = NamedIndices.front();
					NamedIndices.pop_front();
				}

				for ( uint32_t i = 0; i < Loaded->mNumMaterials; ++i )
				{
					if ( i < CurrentNamedIndex )
					{
						if ( LoadedMaterials[ i ].m_Name.Data() == nullptr )
						{
							LoadedMaterials[ i ].m_Name = std::string( Begin->Name.Data() ) + "_material_" + std::to_string( CurrentUnnamedIndex++ );
						}

						continue;
					}

					if ( NamedIndices.size() )
					{
						CurrentNamedIndex = NamedIndices.front();
						NamedIndices.pop_front();
					}
				}
			}

			NamedIndices.clear();

			// Name the meshes.
			if ( LoadedMeshes.size() )
			{
				for ( auto NameBegin = MeshNames.begin(), NameEnd = MeshNames.end(); NameBegin != NameEnd; ++NameBegin )
				{
					LoadedMeshes[ NameBegin->Index ].m_Name = Name( NameBegin->Name );
					NamedIndices.push_back( NameBegin->Index );
				}

				uint32_t CurrentUnnamedIndex = 0, CurrentNamedIndex = -1;

				if ( NamedIndices.size() )
				{
					CurrentNamedIndex = NamedIndices.front();
					NamedIndices.pop_front();
				}

				for ( uint32_t i = 0; i < Loaded->mNumMeshes; ++i )
				{
					if ( i < CurrentNamedIndex )
					{
						if ( LoadedMeshes[ i ].m_Name.Data() == nullptr )
						{
							LoadedMeshes[ i ].m_Name = std::string( Begin->Name.Data() ) + "_mesh_" + std::to_string( CurrentUnnamedIndex++ );
						}

						continue;
					}

					if ( NamedIndices.size() )
					{
						CurrentNamedIndex = NamedIndices.front();
						NamedIndices.pop_front();
					}
				}
			}

			NamedIndices.clear();

			// Name the textures.
			if ( LoadedTexture2Ds.size() )
			{
				for ( auto NameBegin = Texture2DNames.begin(), NameEnd = Texture2DNames.end(); NameBegin != NameEnd; ++NameBegin )
				{
					LoadedTexture2Ds[ NameBegin->Index ].m_Name = Name( NameBegin->Name );
					NamedIndices.push_back( NameBegin->Index );
				}

				uint32_t CurrentUnnamedIndex = 0, CurrentNamedIndex = -1;

				if ( NamedIndices.size() )
				{
					CurrentNamedIndex = NamedIndices.front();
					NamedIndices.pop_front();
				}

				for ( uint32_t i = 0; i < Loaded->mNumTextures; ++i )
				{
					if ( i < CurrentNamedIndex )
					{
						if ( LoadedTexture2Ds[ i ].m_Name.Data() == nullptr )
						{
							LoadedTexture2Ds[ i ].m_Name = std::string( Begin->Name.Data() ) + "_texture2d_" + std::to_string( CurrentUnnamedIndex++ );
						}

						continue;
					}

					if ( NamedIndices.size() )
					{
						CurrentNamedIndex = NamedIndices.front();
						NamedIndices.pop_front();
					}
				}
			}
			
			// Push
			for ( const auto& Material : LoadedMaterials )
			{
				size_t ResourceSize = Serialization::GetSizeOf( Material );
				File ResourceFile = a_TempDirectory.NewFile( ( std::string( Material.m_Name.Data() ) + ".material" ).c_str(), ResourceSize );
				ResourceFile.Open();
				FileSerializer Serializer( ResourceFile );
				Serializer << Material;
				ResourceFile.Close();
			}

			for ( const auto& Mesh : LoadedMeshes )
			{
				size_t ResourceSize = Serialization::GetSizeOf( Mesh );
				File ResourceFile = a_TempDirectory.NewFile( ( std::string( Mesh.m_Name.Data() ) + ".mesh" ).c_str(), ResourceSize );
				ResourceFile.Open();
				FileSerializer Serializer( ResourceFile );
				Serializer << Mesh;
				ResourceFile.Close();
			}

			for ( const auto& Texture2D : LoadedTexture2Ds )
			{
				size_t ResourceSize = Serialization::GetSizeOf( Texture2D );
				File ResourceFile = a_TempDirectory.NewFile( ( std::string( Texture2D.m_Name.Data() ) + ".mesh" ).c_str(), ResourceSize );
				ResourceFile.Open();
				FileSerializer Serializer( ResourceFile );
				Serializer << Texture2D;
				ResourceFile.Close();
			}

			Prefab Temp;
			PrefabHandler( Loaded, Loaded->mRootNode, LoadedMaterials, LoadedMeshes, LoadedTexture2Ds, Temp );
			Temp.m_Name = Begin->Name;
			
			size_t ResourceSize = Serialization::GetSizeOf( Temp );
			File ResourceFile = a_TempDirectory.NewFile( ( std::string( Begin->Name.Data() ) + ".prefab" ).c_str(), ResourceSize );
			ResourceFile.Open();
			FileSerializer Serializer( ResourceFile );
			Serializer << Temp;
			ResourceFile.Close();
			// Push Temp into a file.
		}
		else if ( Begin->Type == "material" )
		{
			Material Temp;
			MaterialHandler( Loaded, Begin->Index, Temp );
			Temp.m_Name = Begin->Name;

			size_t ResourceSize = Serialization::GetSizeOf( Temp );
			File ResourceFile = a_TempDirectory.NewFile( ( std::string( Temp.m_Name.Data() ) + ".material" ).c_str(), ResourceSize );
			ResourceFile.Open();
			FileSerializer Serializer( ResourceFile );
			Serializer << Temp;
			ResourceFile.Close();
		}
		else if ( Begin->Type == "mesh" )
		{
			Mesh Temp;
			MeshHandler( Loaded, Begin->Index, Temp );
			Temp.m_Name =  Begin->Name;

			size_t ResourceSize = Serialization::GetSizeOf( Temp );
			File ResourceFile = a_TempDirectory.NewFile( ( std::string( Temp.m_Name.Data() ) + ".mesh" ).c_str(), ResourceSize );
			ResourceFile.Open();
			FileSerializer Serializer( ResourceFile );
			Serializer << Temp;
			ResourceFile.Close();
		}
		else if ( Begin->Type == "texture2d" )
		{
			Texture2D Temp;
			Texture2DHandler( Loaded, Begin->Index, Temp );
			Temp.m_Name = Begin->Name;

			size_t ResourceSize = Serialization::GetSizeOf( Temp );
			File ResourceFile = a_TempDirectory.NewFile( ( std::string( Temp.m_Name.Data() ) + ".texture2d" ).c_str(), ResourceSize );
			ResourceFile.Open();
			FileSerializer Serializer( ResourceFile );
			Serializer << Temp;
			ResourceFile.Close();
		}
	}

	return true;
}

bool ResourcePackager::CreateTempResources_STBI( const Directory& a_SourceDirectory, const Directory& a_TempDirectory, std::list< LoadInstruction >& a_Instructions )
{
	for ( auto Begin = a_Instructions.begin(), End = a_Instructions.end(); Begin != End; ++Begin )
	{

	}

	return true;
}