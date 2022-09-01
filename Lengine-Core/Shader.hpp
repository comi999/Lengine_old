#pragma once
#include "Math.hpp"
#include "Resource.hpp"
#include "Hash.hpp"

typedef uint32_t ShaderProgramHandle;

enum class ShaderType : uint32_t
{
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	INVALID = uint32_t( -1 )
};

enum class ShaderInfo : uint8_t
{
	COMPILE_STATUS,
	INFO_LOG_LENGTH,
	LINK_STATUS
};

class Shader : public Resource
{
public:

	Shader()
		: Shader( "Default"_N, "API: ConsoleGL VERTEX: Vertex_Default FRAGMENT: Fragment_Default" )
	{ }

	Shader( const Name& a_Name, const std::string& a_Source )
		: Resource( a_Name )
		, m_ShaderProgramHandle( 0 )
	{
		SetSource( a_Source );
	}

	std::string GetAPI()
	{
		return m_API;
	}

	void SetSource( const std::string& a_Source )
	{
		static constexpr auto APITagKey = "API:";
		auto APITagOffset = a_Source.find( APITagKey );

		if ( a_Source.empty() || APITagOffset == std::string::npos )
		{
			m_API = "";
			m_Source = "";
			return;
		}

		APITagOffset = a_Source.find_first_not_of( " \n", APITagOffset + strlen( APITagKey ) );
		auto APITagEnd = a_Source.find_first_of( " \n", APITagOffset );
		m_API = a_Source.substr( APITagOffset, APITagEnd - APITagOffset );
		auto SourceOffset = APITagEnd;
		SourceOffset = a_Source.find_first_not_of( " \n", SourceOffset );

		if ( SourceOffset == std::string::npos )
		{
			m_Source = "";
			return;
		}

		m_Source = a_Source.substr( SourceOffset );
	}

	const std::string& GetSource() const
	{
		return m_Source;
	}

	ShaderProgramHandle GetHandle() const
	{
		return m_ShaderProgramHandle;
	}

	ShaderProgramHandle& GetHandle()
	{
		return m_ShaderProgramHandle;
	}

private:

	friend class ResourcePackager;
	friend class Serialization;

	template < typename T >
	void Serialize( T& a_Serializer ) const
	{
		a_Serializer << *static_cast< Resource* >( this );
		a_Serializer << m_Source;
	}

	template < typename T >
	void Deserialize( T& a_Deserializer )
	{
		a_Deserializer >> *static_cast< Resource* >( this );
		a_Deserializer >> m_Source;
	}

	ShaderProgramHandle m_ShaderProgramHandle;
	std::string         m_API;
	std::string         m_Source;

public:

	static Shader Default;
	static Shader Diffuse;
	static Shader LitDiffuse;
	static Shader Specular;
	static Shader Normal;
	static Shader Phong;
	static Shader UnlitFlatColour;
	static Shader LitFlatColour;
	static Shader Spotlight;
	// More lighting models.
};