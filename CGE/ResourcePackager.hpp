#pragma once
#include "JSON.hpp"
#include "STBI.hpp"
#include "TOBJ.hpp"
#include "OFBX.hpp"
#include "File.hpp"

#include "Texture.hpp"
#include "Mesh.hpp"
#include "Material.hpp"



class ResourcePackager
{
public:

	void BuildPackage()
	{
		/*if ( !Directory().ContainsFile( "manifest.json" ) )
		{

		}

		File ManifestFile = ManifestDir.GetFile( "manifest.json" );

		if ( !ManifestFile.Open() )
		{
			return false;
		}

		Json Manifest;
		ManifestFile.AsIFStream() >> Manifest;

		if ( !Manifest.contains( "Resources" ) )
		{
			return false;
		}

		Directory::SetWorkingDirectory( ManifestDir );

		Json Resources = Manifest[ "Resources" ];

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

			File ResourceFile( Path( FilePath ).GetAbsolute() );

			if ( FileType == "Texture" )
			{
				Texture Texture;
				
				if ( !LoadTexture( Texture, ResourceFile ) )
				{
					continue;
				}

				auto OutputFile = PackagedDir.CreateNewFile( ( Key + ".texture" ).c_str(), Serialization::GetSizeOf( Texture ) );
				OutputFile.Open();

				if ( !OutputFile.IsOpen() )
				{
					return false;
				}

				FileSerializer OutputStream( OutputFile );
				OutputStream << Texture;
				OutputFile.Close();
			}
		}*/
	}

private:

	bool LoadTexture( Texture& o_Texture, File& a_File ) const
	{
		o_Texture.m_Data = ( Colour* )stbi_load( std::string( a_File.GetPath() ).c_str(), &o_Texture.m_Size.x, &o_Texture.m_Size.y, nullptr, 4 );
		return o_Texture.m_Data;
	}
};