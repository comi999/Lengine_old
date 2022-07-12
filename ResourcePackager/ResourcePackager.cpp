#include "ResourcePackager.hpp"
#include <string>
#include "File.hpp"
#include "Name.hpp"
#include "JSON.hpp"

// Assimp (Models/Materials/Animations)
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// STB_image (Textures)
#define STB_IMAGE_IMPLEMENTATION
#include "STBI.hpp"

// Resource types
#include "Prefab.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "AudioClip.hpp"

// Component types
#include "Transform.hpp"
#include "MeshRenderer.hpp"
#include "Alias.hpp"
// #include "AudioSource.hpp"
// #include "AudioListener.hpp"
// #include "AudioProcessor.hpp"

constexpr uint32_t SupportedTypes = 4;

uint32_t ConvertToType( const std::string& a_Extension )
{
	if ( a_Extension == "prefab"    ) return 1; // PREFAB
	if ( a_Extension == "mesh"      ) return 2; // MESH
	if ( a_Extension == "material"  ) return 3; // MATERIAL
	if ( a_Extension == "texture"   ) return 4; // TEXTURE
	if ( a_Extension == "audioclip" ) return 5; // AUDIOCLIP

	return 0; // NONE
}

std::string ConvertToExtension( uint32_t a_Type )
{
	switch ( a_Type )
	{
		case 1: return ".prefab";
		case 2: return ".mesh";
		case 3: return ".material";
		case 4: return ".texture";
		case 5: return ".audioclip";
	}

	return "";
}

uint32_t GetResourceLoader( const std::string& a_File )
{
	File ResourceFile( a_File );

	switch ( CRC32_RT( ResourceFile.GetExtension().c_str() ) )
	{
		case ".obj"_H:  return 1; // ASSIMP
		case ".fbx"_H:  return 1; // ASSIMP
		case ".ply"_H:  return 1; // ASSIMP
		case ".png"_H:  return 2; // STBI
		case ".jpg"_H:  return 2; // STBI
		case ".bmp"_H:  return 2; // STBI
		case ".tga"_H:  return 2; // STBI
		case ".gif"_H:  return 2; // STBI
		case ".jpeg"_H: return 2; // STBI
		case ".jfif"_H: return 2; // STBI
		case ".mp3"_H:  return 3; // AudioClip
		case ".ogg"_H:  return 3; // AudioClip
		case ".flac"_H:  return 3; // AudioClip
		case ".wav"_H:  return 3; // AudioClip
	}

	return 0;
}

inline std::string ConvertPath( const char* a_Path )
{
	return std::filesystem::path( a_Path ).make_preferred().string();
}

// ResourceTypes
// 0 - NONE
// 1 - PREFAB
// 2 - MESH
// 3 - MATERIAL
// 4 - TEXTURE
// 5 - AUDIOCLIP

// ResourceLoaders
// 0 - NONE
// 1 - ASSIMP
// 2 - STBI
// 3 - SOLOUD

// To do later:
// - Need to add naming overrides and proper naming system.
// - Need to make sure that if one loader calls for another 
//   loader to load a file, that common files aren't recompiled 
//   into separate assets, and are just referenced by all.
// - Need to add cameras and lights to model loading with prefabs.

struct ResourceEntry
{
	uint32_t    ResourceType;
	uint32_t    ResourceLoader;
	uint32_t    ResourceIndex;
	std::string ResourceFilePath;
	std::string ResourceName;
	const void* ResourceSource;
};

void ValidateResources( Json& a_Resources )
{
	for ( auto Begin = a_Resources.begin(), End = a_Resources.end(); Begin != End; ++Begin )
	{
		auto ThisPath = Begin->find( "Path" );
		auto ThisType = Begin->find( "Type" );

		if ( ThisPath == Begin->end() || ThisType == Begin->end() )
		{
			a_Resources.erase( Begin );
		}

		auto ThisIndex = Begin->find( "Index" );
		
		if ( ThisIndex == Begin->end() )
		{
			( *Begin )[ "Index" ] = 0;
		}
		else if ( !ThisIndex->is_number_unsigned() )
		{
			*ThisIndex = 0;
		}

		*ThisPath = ConvertPath( ThisPath->get< std::string >().c_str() );
		*ThisType = ConvertToType( ThisType->get< std::string >() );
	}
}

File ProcessResourceEntry( ResourceEntry& a_Entry, Directory& a_TempDirectory )
{
	switch ( a_Entry.ResourceLoader )
	{
		case 1: /*ASSIMP*/
		{
			static Assimp::Importer Importer;
			bool ThisScopeOwnsScene = false;

			if ( !a_Entry.ResourceSource )
			{
				a_Entry.ResourceSource = Importer.ReadFile( 
					a_Entry.ResourceFilePath,
					aiPostProcessSteps::aiProcess_CalcTangentSpace |
					aiPostProcessSteps::aiProcess_Triangulate |
					aiPostProcessSteps::aiProcess_JoinIdenticalVertices |
					aiPostProcessSteps::aiProcess_EmbedTextures |
					aiPostProcessSteps::aiProcess_RemoveRedundantMaterials |
					aiPostProcessSteps::aiProcess_FindInvalidData |
					aiPostProcessSteps::aiProcess_FixInfacingNormals );
				ThisScopeOwnsScene = true;
			}

			const aiScene* ThisScene = ( const aiScene* )a_Entry.ResourceSource;

			switch ( a_Entry.ResourceType )
			{
				case 1: /*PREFAB*/
				{
					std::vector< File > TempMeshes;
					std::vector< File > TempMaterials;
					std::vector< File > TempTextures;

					// Process all subordinates first.
					for ( uint32_t i = 0; i < ThisScene->mNumMeshes; ++i )
					{
						ResourceEntry NewMesh;
						NewMesh.ResourceIndex = i;
						NewMesh.ResourceLoader = 1; /*ASSIMP*/
						NewMesh.ResourceName = a_Entry.ResourceName + "_mesh" + std::to_string( i );
						NewMesh.ResourceType = 2; /*MESH*/
						NewMesh.ResourceSource = a_Entry.ResourceSource;
						TempMeshes.push_back( ProcessResourceEntry( NewMesh, a_TempDirectory ) );
					}

					for ( uint32_t i = 0; i < ThisScene->mNumMaterials; ++i )
					{
						ResourceEntry NewMaterial;
						NewMaterial.ResourceIndex = i;
						NewMaterial.ResourceLoader = 1; /*ASSIMP*/
						NewMaterial.ResourceName = a_Entry.ResourceName + "_material" + std::to_string( i );
						NewMaterial.ResourceType = 3; /*MATERIAL*/
						NewMaterial.ResourceSource = a_Entry.ResourceSource;
						TempMaterials.push_back( ProcessResourceEntry( NewMaterial, a_TempDirectory ) );
					}

					static auto AttachAlias = [&]( Prefab& a_Prefab, const std::string& a_Name )
						{
							Alias Comp;
							Comp.SetName( a_Name );
							a_Prefab.AddComponent( Comp );
						};
					static auto AttachTransform = [&]( Prefab& a_Prefab, const aiMatrix4x4& a_Transform )
						{
							Vector3 Position, Scale;
							Quaternion Rotation;
							const Matrix4& Matrix = *reinterpret_cast< const Matrix4* >( &a_Transform.a1 );
							Matrix4::Decompose( Matrix, Position, Rotation, Scale );
							Transform Comp;
							Comp.SetLocalPosition( Position );
							Comp.SetLocalRotation( Rotation );
							Comp.SetLocalScale( Scale );
							a_Prefab.AddComponent( Comp );
						};
					static auto AttachMeshRenderer = [&]( Prefab& a_Prefab, Hash a_Mesh, Hash a_Material )
						{
							MeshRenderer Comp;
							Comp.SetMesh( ResourceHandle< Mesh >( a_Mesh ) );
							Comp.SetMaterial( ResourceHandle< Material >( a_Material ) );
							a_Prefab.AddComponent( Comp );
						};
					static auto AttachCamera = [&]( Prefab& a_Prefab, aiCamera* a_Camera )
						{
							Camera Comp;
							Comp.SetAspect( a_Camera->mAspect );
							Comp.SetNearZ( a_Camera->mClipPlaneNear );
							Comp.SetFarZ( a_Camera->mClipPlaneFar );
							Comp.SetFOV( a_Camera->mHorizontalFOV );
							a_Prefab.AddComponent( Comp );
						};
					// static auto AttachLight

					// This is for unnamed entries that have incremented names.
					uint32_t Unnamed = 0;

					// Load prefab.
					Action< const aiScene*, aiNode*, Prefab& > AttachMeshRenderers = [&]( const aiScene* a_Scene, aiNode* a_Node, Prefab& o_Prefab )
					{
						std::string ThisName = a_Entry.ResourceName + "_" + ( !a_Node->mName.length ? a_Node->mName.C_Str() : "child" + std::to_string( Unnamed++ ) );
						o_Prefab.SetName( ThisName );
						AttachAlias( o_Prefab, ThisName );
						AttachTransform( o_Prefab, a_Node->mTransformation );

						for ( uint32_t i = 0; i < a_Node->mNumMeshes; ++i )
						{
							Prefab& NewChild = *o_Prefab.AddChild();
							std::string NewChildName = a_Entry.ResourceName + "_child" + std::to_string( Unnamed++ );
							NewChild.SetName( NewChildName );
							AttachAlias( NewChild, NewChildName );
							AttachTransform( NewChild, aiMatrix4x4() );
							uint32_t MeshIndex = a_Node->mMeshes[ i ];
							Hash MeshName = CRC32_RT( TempMeshes[ MeshIndex ].GetStem().c_str() );
							Hash MaterialName = CRC32_RT( TempMaterials[ a_Scene->mMeshes[ MeshIndex ]->mMaterialIndex ].GetStem().c_str() );
							AttachMeshRenderer( o_Prefab, MeshName, MaterialName );
						}

						for ( uint32_t i = 0; i < a_Node->mNumChildren; ++i )
						{
							AttachMeshRenderers( a_Scene, a_Node->mChildren[ i ], *o_Prefab.AddChild() );
						}
					};

					// Create the Prefab.
					Prefab ThisPrefab;
					ThisPrefab.SetName( a_Entry.ResourceName );
					AttachAlias( ThisPrefab, a_Entry.ResourceName );
					AttachTransform( ThisPrefab, aiMatrix4x4() );
					AttachMeshRenderers( ThisScene, ThisScene->mRootNode, *ThisPrefab.AddChild() );
					File ThisTemp = a_TempDirectory.NewFile( ( a_Entry.ResourceName + ConvertToExtension( a_Entry.ResourceType ) ).c_str(), Serialization::GetSizeOf( ThisPrefab ) );
					ThisTemp.Open();
					FileSerializer Serializer( ThisTemp );
					Serializer << ThisPrefab;
					ThisTemp.Close();
					return ThisTemp;

					// TO DO 
					//Unnamed = 0;

					//// This doesn't really work. Need to determine how to properly link cameras and lights.
					//// Create cameras as children of root prefab.
					//for ( uint32_t i = 0; i < ThisScene->mNumCameras; ++i )
					//{
					//	Prefab& ThisCamera = *ThisPrefab.AddChild();
					//	std::string ThisCameraName = a_Entry.ResourceName + "_camera" + std::to_string( Unnamed++ );
					//	ThisCamera.SetName( ThisCameraName );
					//	AttachAlias( ThisCamera, ThisCameraName );
					//	AttachCamera( ThisCamera, ThisScene->mCameras[ i ] );
					//}

					//Unnamed = 0;

					//// Create lights as children of the root prefab.
					//for ( uint32_t i = 0; i < ThisScene->mNumLights; ++i )
					//{
					//	Prefab& ThisLight = *ThisPrefab.AddChild();
					//	std::string ThisLightName = a_Entry.ResourceName + "_light" + std::to_string( Unnamed++ );
					//	ThisLight.SetName( ThisLightName );
					//	AttachAlias( ThisLight, ThisLightName );
					//	// Attachlight
					//}
				}
				case 2: /*MESH*/
				{
					Mesh ThisMesh;
					aiMesh* ThisMeshSource = ThisScene->mMeshes[ a_Entry.ResourceIndex ];
					ThisMesh.SetName( a_Entry.ResourceName );

					if ( ThisMeshSource->HasFaces() )
					{
						uint32_t IndexCount = ThisMeshSource->mNumFaces * 3u;
						ThisMesh.m_Indices.resize( IndexCount );
						aiFace* Face = ThisMeshSource->mFaces;

						for ( uint32_t i = 0, *Begin = ThisMesh.m_Indices.data(), *End = Begin + IndexCount; Begin != End; ++i, Begin += 3, ++Face )
						{
							Begin[ 0 ] = Face->mIndices[ 0 ];
							Begin[ 1 ] = Face->mIndices[ 1 ];
							Begin[ 2 ] = Face->mIndices[ 2 ];
						}
					}

					for ( uint32_t i = 0; i < 8; ++i ) // There is 8 possible colour channels
					{
						if ( ThisMeshSource->HasVertexColors( i ) )
						{
							ThisMesh.m_Colours[ i ].resize( ThisMeshSource->mNumVertices );

							for ( uint32_t j = 0; j < ThisMeshSource->mNumVertices; ++j )
							{
								ThisMesh.m_Colours[ i ][ j ] = {
									ThisMeshSource->mColors[ i ][ j ].r,
									ThisMeshSource->mColors[ i ][ j ].g,
									ThisMeshSource->mColors[ i ][ j ].b,
									ThisMeshSource->mColors[ i ][ j ].a
								};
							};
						}
					}

					if ( ThisMeshSource->HasPositions() )
					{
						ThisMesh.m_Positions.resize( ThisMeshSource->mNumVertices );

						for ( uint32_t i = 0; i < ThisMeshSource->mNumVertices; ++i )
						{
							ThisMesh.m_Positions[ i ] = {
								ThisMeshSource->mVertices[ i ][ 0 ],
								ThisMeshSource->mVertices[ i ][ 1 ],
								ThisMeshSource->mVertices[ i ][ 2 ]
							};
						}
					}

					if ( ThisMeshSource->HasNormals() )
					{
						ThisMesh.m_Normals.resize( ThisMeshSource->mNumVertices );

						for ( uint32_t i = 0; i < ThisMeshSource->mNumVertices; ++i )
						{
							ThisMesh.m_Normals[ i ] = {
								ThisMeshSource->mNormals[ i ][ 0 ],
								ThisMeshSource->mNormals[ i ][ 1 ],
								ThisMeshSource->mNormals[ i ][ 2 ]
							};
						}
					}

					if ( ThisMeshSource->HasTangentsAndBitangents() )
					{
						ThisMesh.m_Tangents.resize( ThisMeshSource->mNumVertices );
						ThisMesh.m_Bitangents.resize( ThisMeshSource->mNumVertices );

						for ( uint32_t i = 0; i < ThisMeshSource->mNumVertices; ++i )
						{
							ThisMesh.m_Tangents[ i ] = {
								ThisMeshSource->mTangents[ i ][ 0 ],
								ThisMeshSource->mTangents[ i ][ 1 ],
								ThisMeshSource->mTangents[ i ][ 2 ]
							};

							ThisMesh.m_Bitangents[ i ] = {
								ThisMeshSource->mBitangents[ i ][ 0 ],
								ThisMeshSource->mBitangents[ i ][ 1 ],
								ThisMeshSource->mBitangents[ i ][ 2 ]
							};
						}
					}

					for ( uint32_t i = 0; i < 8; ++i ) // There is 8 possible texel channels.
					{
						if ( ThisMeshSource->HasTextureCoords( i ) )
						{
							ThisMesh.m_Texels[ i ].resize( ThisMeshSource->mNumVertices );

							for ( uint32_t j = 0; j < ThisMeshSource->mNumVertices; ++j )
							{
								ThisMesh.m_Texels[ i ][ j ] = {
									ThisMeshSource->mTextureCoords[ i ][ j ][ 0 ],
									ThisMeshSource->mTextureCoords[ i ][ j ][ 1 ]
								};
							}
						}
					}

					File ThisTemp = a_TempDirectory.NewFile( ( a_Entry.ResourceName + ConvertToExtension( a_Entry.ResourceType ) ).c_str(), Serialization::GetSizeOf( ThisMesh ) );
					ThisTemp.Open();
					FileSerializer Serializer( ThisTemp );
					Serializer << ThisMesh;
					ThisTemp.Close();
					return ThisTemp;
				}
				case 3: /*MATERIAL*/
				{
					Material ThisMaterial;
					aiMaterial* ThisMaterialSource = ThisScene->mMaterials[ a_Entry.ResourceIndex ];
					ThisMaterial.SetName( a_Entry.ResourceName );

					// Colours
					if ( aiColor4D ColourDiffuse; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_COLOR_DIFFUSE, ColourDiffuse ) )
						ThisMaterial.AddProperty( "diffuse_colour"_N, reinterpret_cast< Vector4& >( ColourDiffuse ) );
					if ( aiColor4D ColourSpecular; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_COLOR_SPECULAR, ColourSpecular ) )
						ThisMaterial.AddProperty( "specular_colour"_N, reinterpret_cast< Vector4& >( ColourSpecular ) );
					if ( aiColor4D ColourAmbient; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_COLOR_AMBIENT, ColourAmbient ) )
						ThisMaterial.AddProperty( "ambient_colour"_N, reinterpret_cast< Vector4& >( ColourAmbient ) );
					if ( aiColor4D ColourEmissive; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_COLOR_EMISSIVE, ColourEmissive ) )
						ThisMaterial.AddProperty( "emissive_colour"_N, reinterpret_cast< Vector4& >( ColourEmissive ) );
					if ( aiColor4D ColourTransparent; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_COLOR_TRANSPARENT, ColourTransparent ) )
						ThisMaterial.AddProperty( "transparent_colour"_N, reinterpret_cast< Vector4& >( ColourTransparent ) );
					if ( aiColor4D ColourReflective; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_COLOR_REFLECTIVE, ColourReflective ) )
						ThisMaterial.AddProperty( "reflective_colour"_N, reinterpret_cast< Vector4& >( ColourReflective ) );
					
					// Values
					if ( float Reflectivity; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_REFLECTIVITY, Reflectivity ) )
						ThisMaterial.AddProperty( "reflectivity"_N, Reflectivity );
					if ( float Opacity; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_OPACITY, Opacity ) )
						ThisMaterial.AddProperty( "opacity"_N, Opacity );
					if ( float Shininess; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_SHININESS, Shininess ) )
						ThisMaterial.AddProperty( "shininess"_N, Shininess );
					if ( float ShininessStrength; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_SHININESS_STRENGTH, ShininessStrength ) )
						ThisMaterial.AddProperty( "shininess_strength"_N, ShininessStrength );
					if ( float RefractionIndex; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_REFRACTI, RefractionIndex ) )
						ThisMaterial.AddProperty( "refraction_index"_N, RefractionIndex );

					// Settings
					if ( int32_t ShadingModel; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_SHADING_MODEL, ShadingModel ) )
						ThisMaterial.AddProperty( "shading_model"_N, ShadingModel );
					if ( int32_t BlendFunc; AI_SUCCESS == ThisMaterialSource->Get( AI_MATKEY_BLEND_FUNC, BlendFunc ) )
						ThisMaterial.AddProperty( "blend_func"_N, BlendFunc );

					// Textures
					static auto AddTextures = [&]( aiTextureType a_Type, const char* a_Name )
					{
						for ( uint32_t i = 0; i < Math::Min( ThisMaterialSource->GetTextureCount( a_Type ), 1u ); ++i )// i < ThisMaterialSource->GetTextureCount( a_Type ); ++i )
						{
							if ( aiString TexturePath; !ThisMaterialSource->Get( AI_MATKEY_TEXTURE( a_Type, i ), TexturePath ) )
							{
								ResourceEntry NewTexture;
								NewTexture.ResourceIndex = ThisScene->GetEmbeddedTextureAndIndex( TexturePath.C_Str() ).second;
								NewTexture.ResourceLoader = 1; /*ASSIMP*/
								NewTexture.ResourceName = a_Entry.ResourceName + "_" + a_Name;// + std::to_string( i );
								NewTexture.ResourceType = 4; /*TEXTURE*/
								NewTexture.ResourceSource = a_Entry.ResourceSource;
								ProcessResourceEntry( NewTexture, a_TempDirectory );
								Name NewTextureName = NewTexture.ResourceName;
								ThisMaterial.AddTexture( Name( a_Name ), { NewTextureName } );
							}
						}
					};

					// Need to add the rest of the properties that aiMaterial contains.
					AddTextures( aiTextureType::aiTextureType_AMBIENT, "texture_ambient" );
					AddTextures( aiTextureType::aiTextureType_AMBIENT_OCCLUSION, "texture_ambient_occlusion" );
					AddTextures( aiTextureType::aiTextureType_BASE_COLOR, "texture_base_colour" );
					AddTextures( aiTextureType::aiTextureType_CLEARCOAT, "texture_clearcoat" );
					AddTextures( aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse" );
					AddTextures( aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, "texture_diffuse_roughness" );
					AddTextures( aiTextureType::aiTextureType_DISPLACEMENT, "texture_displacement" );
					AddTextures( aiTextureType::aiTextureType_EMISSION_COLOR, "texture_emission_colour" );
					AddTextures( aiTextureType::aiTextureType_EMISSIVE, "texture_emissive" );
					AddTextures( aiTextureType::aiTextureType_HEIGHT, "texture_height" );
					AddTextures( aiTextureType::aiTextureType_LIGHTMAP, "texture_lightmap" );
					AddTextures( aiTextureType::aiTextureType_METALNESS, "texture_metalness" );
					AddTextures( aiTextureType::aiTextureType_NORMALS, "texture_normal" );
					AddTextures( aiTextureType::aiTextureType_NORMAL_CAMERA, "texture_normal_camera" );
					AddTextures( aiTextureType::aiTextureType_OPACITY, "texture_opacity" );
					AddTextures( aiTextureType::aiTextureType_REFLECTION, "texture_reflection" );
					AddTextures( aiTextureType::aiTextureType_SHEEN, "texture_sheen" );
					AddTextures( aiTextureType::aiTextureType_SHININESS, "texture_shininess" );
					AddTextures( aiTextureType::aiTextureType_SPECULAR, "texture_specular" );
					AddTextures( aiTextureType::aiTextureType_TRANSMISSION, "texture_transmission" );

					File ThisTemp = a_TempDirectory.NewFile( ( a_Entry.ResourceName + ConvertToExtension( a_Entry.ResourceType ) ).c_str(), Serialization::GetSizeOf( ThisMaterial ) );
					ThisTemp.Open();
					FileSerializer Serializer( ThisTemp );
					Serializer << ThisMaterial;
					ThisTemp.Close();
					return ThisTemp;
				}
				case 4: /*TEXTURE*/
				{
					// Create the Texture.
					Texture2D ThisTexture;
					aiTexture* ThisTextureSource = ThisScene->mTextures[ a_Entry.ResourceIndex ];
					uint8_t*& TextureData = ResourcePackager::GetTextureData( ThisTexture );
					Vector2Int& TextureSize = ResourcePackager::GetTextureSize( ThisTexture );

					if ( !ThisTextureSource->mHeight )
					{
						TextureData = stbi_load_from_memory( ( uint8_t* )ThisTextureSource->pcData, ThisTextureSource->mWidth, &TextureSize.x, &TextureSize.y, nullptr, 4 );
					}
					else
					{
						TextureData = ( uint8_t* )ThisTextureSource->pcData;
						TextureSize = { ThisTextureSource->mWidth, ThisTextureSource->mHeight };
					}

					File ThisTemp = a_TempDirectory.NewFile( ( a_Entry.ResourceName + ConvertToExtension( a_Entry.ResourceType ) ).c_str(), Serialization::GetSizeOf( ThisTexture ) );
					ThisTemp.Open();
					FileSerializer Serializer( ThisTemp );
					Serializer << ThisTexture;
					ThisTemp.Close();
					if ( !ThisTextureSource->mWidth )
					{
						stbi_image_free( TextureData );
						TextureData = nullptr;
					}
					return ThisTemp;
				}
			}

			if ( ThisScopeOwnsScene )
			{
				Importer.FreeScene();
			}

			break;
		}
		case 2: /*STBI*/
		{
			Texture2D ThisTexture;
			uint8_t*& TextureData = ResourcePackager::GetTextureData( ThisTexture );
			Vector2Int& TextureSize = ResourcePackager::GetTextureSize( ThisTexture );
			TextureData = stbi_load( a_Entry.ResourceFilePath.c_str(), &TextureSize.x, &TextureSize.y, nullptr, 4 );
			File ThisTemp = a_TempDirectory.NewFile( ( a_Entry.ResourceName + ConvertToExtension( a_Entry.ResourceType ) ).c_str(), Serialization::GetSizeOf( ThisTexture ) );
			ThisTemp.Open();
			FileSerializer Serializer( ThisTemp );
			Serializer << ThisTexture;
			ThisTemp.Close();
			stbi_image_free( TextureData );
			TextureData = nullptr;
			return ThisTemp;
		}
		case 3: /*AudioClip*/
		{
			AudioClip clip;
            clip.SetName( a_Entry.ResourceName );
			auto path = a_Entry.ResourceFilePath.c_str();
			clip.LoadFromFile(path);

			File ThisTemp = a_TempDirectory.NewFile( ( a_Entry.ResourceName + ConvertToExtension( a_Entry.ResourceType ) ).c_str(), Serialization::GetSizeOf( clip ) );
			ThisTemp.Open();
			FileSerializer Serializer( ThisTemp );
			Serializer << clip;
			ThisTemp.Close();

			return ThisTemp;
		}
	}
}

bool ResourcePackager::Build() const
{
	if ( !m_Source.ContainsFile( "manifest.json" ) ) return false;
	File ManifestFile = m_Source.GetFile( "manifest.json" );
	if ( !ManifestFile.Open() ) return false;
	std::ifstream ManifestStream = ManifestFile;
	Json ManifestJson; ManifestStream >> ManifestJson;
	ManifestFile.Close();
	if ( !ManifestJson.contains( "Resources" ) ) return false;
	Json& Resources = ManifestJson[ "Resources" ];
	ValidateResources( Resources );
	if ( Resources.empty() ) return false;
	if ( m_Output.ContainsDirectory( "Temp" ) ) m_Output.DestroyDirectory( "Temp" );
	Directory TempDirectory = m_Output.NewDirectory( "Temp" );

	// Compile temp files.
	for ( auto Begin = Resources.begin(), End = Resources.end(); Begin != End; ++Begin )
	{
		ResourceEntry Entry;
		Entry.ResourceFilePath = Begin->find( "Path" )->get< std::string >();
		Entry.ResourceLoader = GetResourceLoader( Entry.ResourceFilePath );
		Entry.ResourceIndex = Begin->find( "Index" )->get< uint32_t >();
		Entry.ResourceName = Begin.key();
		Entry.ResourceType = Begin->find( "Type" )->get< uint32_t >();
		Entry.ResourceSource = nullptr;
		ProcessResourceEntry( Entry, TempDirectory );
	}

	// Compile resource.package
	if ( m_Output.ContainsFile( "resource.package" ) ) m_Output.DestroyFile( "resource.package" );
	std::vector< std::tuple< Name, size_t, std::string > > ResourceHeader;
	size_t ResourcesSize = 0;

	{
		DirectoryIterator Begin( TempDirectory );

		for ( ; Begin; ++Begin )
		{
			File ResourceFile( *Begin );
			Name ResourceName( ResourceFile.GetStem() );
			auto& NewHeader = ResourceHeader.emplace_back( ResourceName, ResourcesSize, "" );
			std::string ResourceExtension = ResourceFile.GetExtension();

			if      ( ResourceExtension == ".mesh"      ) std::get< 2 >( NewHeader ) = typeid( Mesh      ).name();
			else if ( ResourceExtension == ".material"  ) std::get< 2 >( NewHeader ) = typeid( Material  ).name();
			else if ( ResourceExtension == ".texture"   ) std::get< 2 >( NewHeader ) = typeid( Texture2D ).name();
			else if ( ResourceExtension == ".prefab"    ) std::get< 2 >( NewHeader ) = typeid( Prefab    ).name();
			else if ( ResourceExtension == ".audioclip" ) std::get< 2 >( NewHeader ) = typeid( AudioClip ).name();

			ResourceFile.Open();
			ResourcesSize += ResourceFile.Size();
			ResourceFile.Close();
		}
	}

	size_t HeaderSize = Serialization::GetSizeOf( ResourceHeader );
	for ( auto& Entry : ResourceHeader ) std::get< 1 >( Entry ) += HeaderSize;
	ResourcesSize += HeaderSize;
	File OutputFile = m_Output.NewFile( "resource.package", ResourcesSize );
	OutputFile.Open();
	FileSerializer Serializer( OutputFile );
	Serializer << ResourceHeader;

	{
		DirectoryIterator Begin( TempDirectory );

		for ( ; Begin; ++Begin )
		{
			File ResourceFile( *Begin );
			ResourceFile.Open();
			std::vector< uint8_t > TempBuffer;
			TempBuffer.resize( ResourceFile.Size() );
			ResourceFile.Read( TempBuffer.data(), TempBuffer.size() );
			ResourceFile.Close();
			Serializer.Stream().Write( TempBuffer.data(), TempBuffer.size() );
		}
	}

	OutputFile.Close();
	m_Output.DestroyDirectory( "Temp" );
}