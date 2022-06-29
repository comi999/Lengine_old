#pragma once
#include <string>
#include "File.hpp"
#include "Name.hpp"
#include "JSON.hpp"

#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Prefab;
class Texture2D;
class Mesh;
class Material;

class ResourcePackager
{
public:

	ResourcePackager() = default;

	ResourcePackager( const Directory& a_SourceDirectory )
		: m_SourceDirectory( a_SourceDirectory )
	{ }

	bool BuildPackage( const Directory& a_OutputDirectory );

private:

	class NameList
	{
	public:

		class Entry
		{
		public:

			Entry() = default;

			Entry( const std::string& a_String, uint32_t a_Index, const std::string& a_Name )
				: Type( a_String )
				, Index( a_Index )
				, Name( a_Name )
			{ }

			std::string Type;
			uint32_t    Index;
			std::string Name;

			bool operator<( const Entry& a_Entry ) const
			{
				if ( Type != a_Entry.Type )
				{
					return Type < a_Entry.Type;
				}

				if ( Index < a_Entry.Index )
				{
					return Index < a_Entry.Index;
				}

				return true;
			}
		};

		std::list< Entry > NamingEntries;
	};

private:

	struct LoadInstruction
	{
		std::string File;
		std::string Type;
		Name        Name;
		uint32_t    Index;
		NameList    Names;

		inline bool operator<( const LoadInstruction& a_Instruction )
		{
			if ( File != a_Instruction.File )
			{
				return File < a_Instruction.File;
			}

			if ( Type != a_Instruction.Type )
			{
				return Type < a_Instruction.Type;
			}

			if ( Index < a_Instruction.Index )
			{
				return Index < a_Instruction.Index;
			}

			return true;
		}
	};

	bool ProcessResourceEntry( const std::string& a_Key, const Json& a_Entry );
	static bool CreateTempResources_Assimp( const Directory& a_SourceDirectory, const Directory& a_TempDirectory, std::list< LoadInstruction >& a_Instructions );
	static bool CreateTempResources_STBI  ( const Directory& a_SourceDirectory, const Directory& a_TempDirectory, std::list< LoadInstruction >& a_Instructions );

	/*template < typename T >
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
			a_TempDirectory.DestroyFile( a_TempFile.c_str() );
		}

		std::string TypeString( typeid( T ).name() );
		size_t ResourceSize = Serialization::GetSizeOf( TypeString );
		ResourceSize += Serialization::GetSizeOf( Resource );
		auto OutputFile = a_TempDirectory.NewFile( a_TempFile.c_str(), ResourceSize );

		if ( !OutputFile.Open() )
		{
			return 0;
		}

		FileSerializer OutputStream( OutputFile );
		OutputStream << TypeString << Resource;
		OutputFile.Close();
		return ResourceSize;
	}*/

	Directory                    m_SourceDirectory;
	Directory                    m_TempDirectory;
	std::list< LoadInstruction > m_LoadInstructions;
};